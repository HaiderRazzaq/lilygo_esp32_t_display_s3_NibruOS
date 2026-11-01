import sys
import struct
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
from check_resources import inspect_image


class ResourceChecks(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.image = Path(self.tmp.name) / 'firmware.bin'
        self.partitions = Path(self.tmp.name) / 'partitions.bin'
        self.image.write_bytes(bytes(100))
        self.partitions.write_bytes(struct.pack('<HBBII16sI', 0x50AA, 0, 0x10, 0x10000, 120, b'app0', 0))

    def test_partition_fit_and_headroom(self):
        r = inspect_image(self.image, self.partitions, 100)
        self.assertEqual(r['remaining_bytes'], 20)
        self.assertFalse(r['warnings'])

    def test_overflow_is_failure(self):
        self.image.write_bytes(bytes(121))
        with self.assertRaisesRegex(ValueError, 'partition'):
            inspect_image(self.image, self.partitions)

    def test_growth_warning_is_not_failure(self):
        r = inspect_image(self.image, self.partitions, 90)
        self.assertTrue(r['warnings'])
        self.assertGreater(r['growth_percent'], 5)

    def test_missing_app0_cannot_pass(self):
        self.partitions.write_bytes(bytes(32))
        with self.assertRaises(ValueError):
            inspect_image(self.image, self.partitions)

    def test_exact_partition_fit(self):
        self.image.write_bytes(bytes(120))
        self.assertEqual(inspect_image(self.image, self.partitions)['remaining_bytes'], 0)

    def test_empty_image_is_not_a_valid_build(self):
        self.image.write_bytes(b'')
        with self.assertRaises(ValueError):
            inspect_image(self.image, self.partitions)


if __name__ == '__main__':
    unittest.main()
