"""PlatformIO post-image check against the generated app0 partition."""
Import('env')
import subprocess
from pathlib import Path


def check_image(source, target, env):
    root = Path(env.subst('$PROJECT_DIR'))
    build = Path(env.subst('$BUILD_DIR'))
    subprocess.run([env.subst('$PYTHONEXE'), str(root / 'tools/check_resources.py'),
                    '--image', str(build / 'firmware.bin'),
                    '--partitions', str(build / 'partitions.bin'),
                    '--report', str(build / 'resource-report.json')], check=True)


env.AddPostAction('$BUILD_DIR/${PROGNAME}.bin', check_image)
