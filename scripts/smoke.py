#!/usr/bin/env python3
"""Exercise the real executable, including its DI composition root."""
import argparse
import json
import subprocess
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('executable', type=Path)
parser.add_argument('--output-dir', type=Path)
args = parser.parse_args()
cases = [
    ('demo', '8\n0\n', ['Всего на балансе: 4', '5.00 кВт·ч',
                          '1001 | Bicycle | простота 6', 'Техосмотр: отказ.']),
    ('manual', '1\n1\n6\n30\n2\n2\n5\n70\n2.5\n3\n3\n4\n4\n3.25\n5\n6\n7\n0\n',
     ['Всего на балансе: 4', '5.75 кВт·ч', '1 | Bicycle | простота 6']),
    ('reject_both', '1\n1\n10\n31\n2\n2\n10\n69\n50\n5\n6\n7\n0\n',
     ['Техосмотр: отказ.', 'Всего на балансе: 0', '0.00 кВт·ч', 'Список пуст.']),
    ('empty', '5\n6\n7\n0\n', ['Всего на балансе: 0', 'Список пуст.']),
    ('bad_choice', 'oops\n\n2junk\n99999999999999999999999\n42\n0\n',
     ['Нужно одно число', 'Неизвестный пункт', 'До свидания.']),
    ('invalid_values', '3\n0\n1\n1\n11\n20\n2\n2\n6\n101\n1\n4\n3\n-1\n5\n0\n',
     ['Ошибка:', 'Всего на балансе: 0']),
    ('duplicate', '3\n10\n3\n10\n5\n0\n', ['already in use', 'Всего на балансе: 1']),
    ('fractional_and_nonfinite_input', '4\n20\nnan\ninf\n1,5\n1.5\n5\n0\n',
     ['Нужно одно число', 'Всего на балансе: 1', '1.50 кВт·ч']),
    ('eof_menu', '', ['EOF']),
    ('eof_during_admission', '2\n1\n6\n', ['EOF']),
    ('repeated_demo', '8\n8\n5\n0\n', ['Demo needs free', 'Всего на балансе: 4']),
]
results = []
for name, input_text, expected in cases:
    process = subprocess.run([str(args.executable.resolve())], input=input_text,
                             capture_output=True, encoding='utf-8', timeout=5)
    missing = [text for text in expected if text not in process.stdout]
    passed = process.returncode == 0 and not missing
    result = {'name': name, 'passed': passed, 'exit_code': process.returncode,
              'missing_output': missing}
    results.append(result)
    if args.output_dir:
        args.output_dir.mkdir(parents=True, exist_ok=True)
        (args.output_dir / f'{name}.txt').write_text(
            'INPUT:\n' + input_text + '\nOUTPUT:\n' + process.stdout +
            '\nSTDERR:\n' + process.stderr, encoding='utf-8')
    print(f'{name}: {"PASS" if passed else "FAIL"}')
if args.output_dir:
    (args.output_dir / 'summary.json').write_text(json.dumps(results, indent=2) + '\n')
raise SystemExit(0 if all(result['passed'] for result in results) else 1)
