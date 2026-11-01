"""Mutation tests: reject plausible documentation mistakes, not just missing files."""
import shutil
import json
import re
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from check_docs import validate  # noqa: E402


class DocumentationChecks(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / "repo"
        shutil.copytree(ROOT, self.root, ignore=shutil.ignore_patterns(
            ".git", ".pio", ".testbuild", ".build", "build", "__pycache__"))

    def edit(self, path, old, new):
        target = self.root / path
        text = target.read_text(encoding="utf-8")
        self.assertIn(old, text)
        target.write_text(text.replace(old, new, 1), encoding="utf-8")

    def assert_rejected(self, fragment):
        self.assertTrue(any(fragment in error for error in validate(self.root)),
                        validate(self.root))

    def edit_record(self, path, ident, **updates):
        target = self.root / path
        found = False

        def change(match):
            nonlocal found
            record = json.loads(match[1])
            if record.get("id") == ident:
                record.update(updates)
                found = True
            return "```json\n" + json.dumps(record, indent=2, ensure_ascii=False) + "\n```"

        content = re.sub(r"```json\n(.*?)\n```", change,
                         target.read_text(encoding="utf-8"), flags=re.DOTALL)
        self.assertTrue(found, ident)
        target.write_text(content, encoding="utf-8")

    def test_real_repository_is_valid(self):
        self.assertEqual([], validate(self.root))

    def test_missing_required_document(self):
        (self.root / "docs/CURRENT_STATE.md").unlink()
        self.assert_rejected("required")

    def test_broken_link_and_anchor(self):
        with (self.root / "README.md").open("a", encoding="utf-8") as stream:
            stream.write("\n[bad](docs/missing.md)\n[bad anchor](docs/CURRENT_STATE.md#missing-anchor)\n")
        errors = validate(self.root)
        self.assertTrue(any("missing.md" in e for e in errors))
        self.assertTrue(any("missing-anchor" in e for e in errors))

    def test_broken_reference_style_link(self):
        with (self.root / "README.md").open("a", encoding="utf-8") as stream:
            stream.write("\n[bad][destination]\n\n[destination]: docs/missing-reference.md\n")
        self.assert_rejected("missing-reference.md")

    def test_invalid_feature_id(self):
        self.edit("docs/FEATURE_REGISTRY.md", '"id": "FEAT-0001"', '"id": "FEAT-1"')
        self.assert_rejected("invalid ID")

    def test_duplicate_id(self):
        self.edit("docs/FEATURE_REGISTRY.md", '"id": "FEAT-0002"', '"id": "FEAT-0001"')
        self.assert_rejected("duplicate ID")

    def test_unknown_issue_reference(self):
        self.edit("docs/FEATURE_REGISTRY.md", '"BUG-0001"', '"BUG-9999"')
        self.assert_rejected("unknown reference BUG-9999")

    def test_missing_source_reference(self):
        self.edit("docs/FEATURE_REGISTRY.md", '"src/apps/SystemInfoApp.cpp"', '"src/apps/Missing.cpp"')
        self.assert_rejected("Missing.cpp")

    def test_file_rename_preserves_immutable_development_history(self):
        old, new = "src/services/WiFiService.cpp", "src/services/RadioService.cpp"
        (self.root / old).rename(self.root / new)
        # Current feature/issue records follow the rename; historical DEV records
        # must retain the original path without becoming invalid.
        for filename in ("docs/FEATURE_REGISTRY.md", "docs/KNOWN_ISSUES.md"):
            path = self.root / filename
            path.write_text(path.read_text(encoding="utf-8").replace(old, new), encoding="utf-8")
        self.assertEqual([], validate(self.root))

    def test_state_disagrees_with_registry(self):
        self.edit("docs/CURRENT_STATE.md", '"FEAT-0002"', '"FEAT-0007"')
        self.assert_rejected("implemented_features")

    def test_new_app_requires_registry_update(self):
        with (self.root / "src/main.cpp").open("a", encoding="utf-8") as stream:
            stream.write("\n// fixture\nvoid registerExtra() { launcher.registerApp(extra); }\n")
        self.assert_rejected("registered apps")

    def test_hardware_pass_requires_evidence(self):
        self.edit_record("docs/HARDWARE_TEST_LOG.md", "HWT-0001", evidence=[])
        self.assert_rejected("hardware PASS requires")

    def test_hardware_log_rejects_software_as_hardware(self):
        self.edit("docs/HARDWARE_TEST_LOG.md", '"verification_type": "HARDWARE"', '"verification_type": "HOST"')
        self.assert_rejected("must use HARDWARE")

    def test_hardware_record_must_stay_in_dedicated_log(self):
        source = self.root / "docs/HARDWARE_TEST_LOG.md"
        content = source.read_text(encoding="utf-8")
        match = re.search(r"```json\n.*?\n```", content, re.DOTALL)
        source.write_text(content.replace(match[0], "", 1), encoding="utf-8")
        with (self.root / "docs/VERIFICATION_REPORT.md").open("a", encoding="utf-8") as stream:
            stream.write("\n" + match[0] + "\n")
        self.assert_rejected("physical tests belong in HARDWARE_TEST_LOG")

    def test_hardware_pass_requires_observation(self):
        self.edit("docs/HARDWARE_TEST_LOG.md", '"actual": "Owner reported display and CST816S operation working in the independent sketch."', '"actual": "UNKNOWN"')
        self.assert_rejected("hardware PASS requires")

    def test_invalid_test_outcome(self):
        self.edit("docs/HARDWARE_TEST_LOG.md", '"result": "PASS"', '"result": "SUCCESS"')
        self.assert_rejected("invalid result")

    def test_resource_policy_documents_are_required(self):
        for name in ('docs/RESOURCE_BUDGET.md', 'docs/MEMORY_PROFILING.md',
                     'docs/performance/BASELINE.md'):
            with self.subTest(name=name):
                target = self.root / name
                content = target.read_text(encoding='utf-8')
                target.unlink()
                self.assert_rejected('required document')
                target.write_text(content, encoding='utf-8')


if __name__ == "__main__":
    unittest.main()
