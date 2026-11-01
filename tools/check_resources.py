"""Check build artifacts only. Never infer runtime RAM, stack or power from these."""
import argparse
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[1]


def inspect_image(image, partitions, baseline_bytes=None):
    capacity = None
    data = Path(partitions).read_bytes()
    for offset in range(0, len(data) - 31, 32):
        magic, kind, subtype, _, size, name, _ = struct.unpack_from('<HBBII16sI', data, offset)
        if magic == 0x50AA and kind == 0 and subtype == 0x10 and name.rstrip(b'\0') == b'app0':
            capacity = size
            break
    if not capacity:
        raise ValueError('No app0 application partition in generated partition table')
    binary = Path(image).read_bytes()
    used = len(binary)
    if not used or used > capacity:
        raise ValueError(f'Firmware image {used} bytes invalid or exceeds app0 partition {capacity}')
    growth = ((used - baseline_bytes) * 100 / baseline_bytes) if baseline_bytes else None
    return dict(binary_bytes=used, application_partition_bytes=capacity,
                remaining_bytes=capacity - used,
                sha256=hashlib.sha256(binary).hexdigest(), growth_percent=growth,
                warnings=['Image growth exceeds provisional 5% review trigger'] if growth is not None and growth > 5 else [],
                runtime_metrics='NOT MEASURED')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    build = ROOT / '.pio/build/lilygo-t-display-s3'
    parser.add_argument('--image', type=Path, default=build / 'firmware.bin')
    parser.add_argument('--partitions', type=Path, default=build / 'partitions.bin')
    parser.add_argument('--baseline', type=Path, default=ROOT / 'docs/performance/build-baseline.json')
    parser.add_argument('--report', type=Path)
    args = parser.parse_args()
    try:
        baseline = json.loads(args.baseline.read_text())['binary_bytes']
        result = inspect_image(args.image, args.partitions, baseline)
    except (OSError, ValueError, KeyError) as exc:
        print(f'Resource artifact check FAIL: {exc}')
        return 1
    rendered = json.dumps(result, indent=2)
    print(rendered)
    if args.report:
        args.report.write_text(rendered + '\n', encoding='utf-8')
    print('Resource artifact check PASS (runtime acceptance pending)')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
