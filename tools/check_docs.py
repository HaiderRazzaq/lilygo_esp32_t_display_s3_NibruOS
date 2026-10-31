#!/usr/bin/env python3
"""Read-only, standard-library validation of NibruOS engineering records.

This checks structure and evidence references, never the truth of a device report.
Run from anywhere: python /path/to/repository/tools/check_docs.py
"""
import argparse
import json
import re
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
REQUIRED = (
    "AGENTS.md", "README.md", "CHANGELOG.md", "docs/README.md",
    "docs/PROJECT_CONTEXT.md", "docs/CURRENT_STATE.md", "docs/ARCHITECTURE.md",
    "docs/HARDWARE_PINOUT.md", "docs/HARDWARE_INVENTORY.md",
    "docs/DEVELOPMENT_LOG.md", "docs/HARDWARE_TEST_LOG.md", "docs/TEST_MATRIX.md",
    "docs/KNOWN_ISSUES.md", "docs/FEATURE_REGISTRY.md", "docs/DEPENDENCIES.md",
    "docs/ENGINEERING_WORKFLOW.md", "docs/VERIFICATION_REPORT.md",
    "docs/decisions/README.md", "docs/releases/README.md",
)
IGNORED = {".git", ".pio", ".testbuild", "__pycache__", "node_modules"}
ID = re.compile(r"(?:FEAT|BUG|DEV|HWT|VER)-\d{4}\b")
FIELDS = {
    "feature": "id name purpose module files dependencies hardware gpio lifecycle implementation verification issues history",
    "issue": "id title status description symptoms reproduction affected_versions suspected_cause root_cause files attempts fix verification risks",
    "development": "id date related action files description reason expected actual verification risks commit follow_up",
    "test": "id date firmware_version commit verification_type scope hardware wiring procedure expected actual result evidence evidence_kind environment limitations",
    "state": "date runtime_version source_patch_label branch_at_baseline baseline_commit working_tree implemented_features planned_features active_issues latest_build current_host_attempt source_constants",
}
PREFIXES = {"feature": "FEAT", "issue": "BUG", "development": "DEV"}
FEATURE_STATES = {"PLANNED", "IN DEVELOPMENT", "IMPLEMENTED", "DEPRECATED", "REMOVED"}
ISSUE_STATES = {"OPEN", "INVESTIGATING", "BLOCKED", "FIXED — UNVERIFIED", "VERIFIED RESOLVED", "WONTFIX"}
RESULTS = {"PASS", "FAIL", "BLOCKED", "NOT RUN", "SKIPPED"}
TYPES = {"BUILD", "HOST", "STATIC", "DOCUMENTATION", "HARDWARE"}


def without_fences(text):
    return re.sub(r"^(`{3,}|~{3,})[^\n]*\n.*?^\1\s*$", "", text,
                  flags=re.MULTILINE | re.DOTALL)


def anchors(text):
    """GitHub-style heading anchors, including duplicate-heading suffixes."""
    result, counts = set(), {}
    for title in re.findall(r"^#{1,6}\s+(.+?)\s*#*\s*$", without_fences(text), re.MULTILINE):
        title = re.sub(r"\[([^]]+)\]\([^)]*\)", r"\1", title)
        slug = re.sub(r"[^\w\- ]", "", title.lower()).replace(" ", "-")
        number = counts.get(slug, 0)
        counts[slug] = number + 1
        result.add(f"{slug}-{number}" if number else slug)
    result.update(re.findall(r'<a\s+(?:id|name)=[\"\']([^\"\']+)', text))
    return result


def destinations(text):
    text = re.sub(r"`[^`\n]+`", "", without_fences(text))
    # Inline links/images and reference-style definitions; no external fetching.
    inline = re.findall(r"!?\[[^]\n]*\]\(\s*(<[^>]+>|[^)\s]+)(?:\s+[^)]*)?\)", text)
    definitions = re.findall(r"^\s{0,3}\[[^]]+\]:\s*(<[^>]+>|\S+)", text, re.MULTILINE)
    return [target.strip("<>") for target in inline + definitions]


def strings(value):
    if isinstance(value, str):
        yield value
    elif isinstance(value, dict):
        for key, item in value.items():
            if key != "id":
                yield from strings(item)
    elif isinstance(value, list):
        for item in value:
            yield from strings(item)


def validate(root=ROOT):
    root = Path(root).resolve()
    errors, records, definitions = [], [], {}
    texts = {}
    for path in root.rglob("*.md"):
        if not IGNORED.intersection(path.relative_to(root).parts):
            texts[path] = path.read_text(encoding="utf-8-sig")
    for name in REQUIRED:
        if not (root / name).is_file():
            errors.append(f"missing required document: {name}")

    for path, body in texts.items():
        label = path.relative_to(root).as_posix()
        for target in destinations(body):
            parts = urlsplit(target)
            if parts.scheme or parts.netloc:
                continue
            linked = (path.parent / unquote(parts.path)).resolve() if parts.path else path
            if not linked.is_relative_to(root) or not linked.exists():
                errors.append(f"{label}: broken internal link {target}")
            elif parts.fragment and linked.suffix.lower() == ".md":
                linked_text = texts.get(linked, linked.read_text(encoding="utf-8-sig"))
                if unquote(parts.fragment) not in anchors(linked_text):
                    errors.append(f"{label}: missing anchor {target}")

        for block in re.findall(r"^```json\s*\n(.*?)^```\s*$", body, re.MULTILINE | re.DOTALL):
            try:
                payload = json.loads(block)
            except json.JSONDecodeError as exc:
                errors.append(f"{label}: invalid JSON record: {exc}")
                continue
            for rec in payload if isinstance(payload, list) else [payload]:
                if not isinstance(rec, dict) or rec.get("record_type") not in FIELDS:
                    errors.append(f"{label}: missing or invalid record_type")
                    continue
                kind = rec["record_type"]
                records.append((label, rec))
                missing = set(FIELDS[kind].split()) - rec.keys()
                if missing:
                    errors.append(f"{label}: {rec.get('id', kind)} missing fields {sorted(missing)}")
                if kind == "state":
                    continue
                ident = rec.get("id", "")
                prefix = PREFIXES.get(kind, "(?:HWT|VER)")
                if not isinstance(ident, str) or not re.fullmatch(prefix + r"-\d{4}", ident):
                    errors.append(f"{label}: invalid ID {ident}")
                    continue
                if ident in definitions:
                    errors.append(f"{label}: duplicate ID {ident}")
                definitions[ident] = rec

    for label, rec in records:
        kind = rec["record_type"]
        name = rec.get("id", kind)
        for text in strings(rec):
            for ref in ID.findall(text):
                if ref not in definitions:
                    errors.append(f"{label}: {name} unknown reference {ref}")
        for key in ("files", "evidence"):
            if key not in rec:
                continue
            if not isinstance(rec[key], list):
                errors.append(f"{label}: {name} {key} must be an array of paths")
                continue
            for filename in rec[key]:
                if not isinstance(filename, str):
                    errors.append(f"{label}: {name} invalid {key} path {filename}")
                    continue
                target = (root / filename).resolve()
                # DEV paths describe past changes, including removal and renames.
                # Requiring those files today would force historical log rewrites.
                historical = kind == "development" and key == "files"
                if not target.is_relative_to(root) or (not historical and not target.is_file()):
                    errors.append(f"{label}: {name} missing {key} file {filename}")
        if kind == "feature" and rec.get("implementation") not in FEATURE_STATES:
            errors.append(f"{label}: {name} invalid implementation state")
        if kind == "issue" and rec.get("status") not in ISSUE_STATES:
            errors.append(f"{label}: {name} invalid issue status")
        if kind == "test":
            if rec.get("result") not in RESULTS:
                errors.append(f"{label}: {name} invalid result")
            if rec.get("verification_type") not in TYPES:
                errors.append(f"{label}: {name} invalid verification_type")
            if label == "docs/HARDWARE_TEST_LOG.md" or str(name).startswith("HWT-"):
                if rec.get("verification_type") != "HARDWARE":
                    errors.append(f"{label}: {name} must use HARDWARE verification_type")
                if label != "docs/HARDWARE_TEST_LOG.md" or not str(name).startswith("HWT-"):
                    errors.append(f"{label}: {name} physical tests belong in HARDWARE_TEST_LOG with HWT IDs")
            elif rec.get("verification_type") == "HARDWARE":
                errors.append(f"{label}: {name} physical tests belong in HARDWARE_TEST_LOG with HWT IDs")
            if rec.get("verification_type") == "HARDWARE" and rec.get("result") == "PASS":
                unknown = {"", "UNKNOWN", "NOT RUN", "NOT VERIFIED", "PENDING"}
                if (not rec.get("evidence") or str(rec.get("actual", "")).strip() in unknown
                        or str(rec.get("evidence_kind", "")).strip() in unknown):
                    errors.append(f"{label}: {name} hardware PASS requires evidence, provenance and actual observation")

    states = [r for _, r in records if r["record_type"] == "state"]
    features = [r for _, r in records if r["record_type"] == "feature"]
    if len(states) != 1:
        errors.append("CURRENT_STATE must define exactly one state record")
    else:
        state = states[0]
        for key, status in (("implemented_features", "IMPLEMENTED"), ("planned_features", "PLANNED")):
            expected = {r.get("id") for r in features if r.get("implementation") == status}
            actual = state.get(key, [])
            if not isinstance(actual, list) or set(actual) != expected or len(actual) != len(set(actual)):
                errors.append(f"CURRENT_STATE {key} disagrees with FEATURE_REGISTRY")
        board = root / "include/BoardConfig.h"
        if board.exists():
            board_text = board.read_text(encoding="utf-8")
            for constant, value in state.get("source_constants", {}).items():
                match = re.search(r"\b" + re.escape(constant) + r"\s*=\s*(\d+)\s*;", board_text)
                if not match or int(match[1]) != value:
                    errors.append(f"CURRENT_STATE source constant {constant} disagrees with BoardConfig")
        main = root / "src/main.cpp"
        if main.exists():
            source = re.sub(r"/\*.*?\*/|//[^\n]*", "", main.read_text(encoding="utf-8"), flags=re.DOTALL)
            registered = re.findall(r"launcher\.registerApp\(\s*(\w+)\s*\)", source)
            documented = [r["registration"] for r in features
                          if "registration" in r and r.get("implementation") == "IMPLEMENTED"]
            if sorted(registered) != sorted(documented):
                errors.append("FEATURE_REGISTRY registered apps disagree with src/main.cpp")
            version = re.search(r"\[NibruOS\] boot (v[\d.]+)", source)
            if not version or version[1] != state.get("runtime_version"):
                errors.append("CURRENT_STATE runtime_version disagrees with src/main.cpp")
    return errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT, help="repository root (default: this script's repository)")
    args = parser.parse_args()
    errors = validate(args.root)
    for error in errors:
        print(f"ERROR: {error}")
    print(f"Documentation checks: {'FAIL (' + str(len(errors)) + ' errors)' if errors else 'PASS'}")
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
