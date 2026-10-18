#!/usr/bin/env python3
"""Measure unique executable src/ lines with GNU gcov JSON; merge inline copies."""
import argparse
import gzip
import json
import subprocess
import tempfile
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('build', type=Path)
parser.add_argument('--output', type=Path, default=Path('coverage-summary.json'))
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
source = root / 'src'
notes = list(args.build.resolve().rglob('*.gcno'))
if not notes:
    parser.error('No .gcno files: configure with -DVYSHKAT_COVERAGE=ON and run tests')
lines = {}
with tempfile.TemporaryDirectory(prefix='vyshkat-gcov-') as temporary:
    work = Path(temporary)
    for note in notes:
        result = subprocess.run(['gcov', '--json-format', str(note)], cwd=work,
                                capture_output=True, text=True, check=True)
        for archive in work.glob('*.gcov.json.gz'):
            with gzip.open(archive, 'rt') as stream:
                data = json.load(stream)
            for file in data['files']:
                path = Path(file['file']).resolve()
                if not path.is_relative_to(source):
                    continue
                relative = str(path.relative_to(root))
                file_lines = lines.setdefault(relative, {})
                for line in file['lines']:
                    number = line['line_number']
                    file_lines[number] = file_lines.get(number, False) or line['count'] > 0
            archive.unlink()
files = {name: {'covered': sum(values.values()), 'executable': len(values)}
         for name, values in sorted(lines.items())}
covered = sum(file['covered'] for file in files.values())
total = sum(file['executable'] for file in files.values())
if not total:
    raise SystemExit('No executable production source lines found')
summary = {'metric': 'unique executable line coverage', 'scope': 'src/*.cpp and src/*.hpp',
           'excluded': ['tests', 'third_party', 'system headers'],
           'covered': covered, 'executable': total,
           'percent': round(100 * covered / total, 2), 'files': files}
args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
print(json.dumps(summary, indent=2))
raise SystemExit(0 if covered / total >= 0.6 else 1)
