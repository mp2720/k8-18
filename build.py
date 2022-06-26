#!/usr/bin/python
import os
import sys


class Module:
    errors_count = 0
    total_count = 0
    done_count = 0

    def try_exec(self, func: str) -> bool:
        if hasattr(self, func):
            Module.total_count += 1
            try:
                print(f"- Running '{func}'...")
                getattr(self, func)()
                Module.done_count += 1
                print(f"- Done")
            except Exception as e:
                Module.errors_count += 1
                print(f'- Error: {e}')
                print(e.__traceback__)

            return True
        else:
            return False

    @classmethod
    def report(cls):
        print(f'[ERRORS: {cls.errors_count}] [OK: {cls.done_count}] [TOTAL: {cls.total_count}]')


class Logisim(Module):
    CIRC_NAME = 'cpu.circ'
    CIRCTMPL_NAME = 'cpu.circtmpl'

    def __init__(self, cd_path: str):
        self.cd_path = cd_path

    def tmpl(self):
        """
        Создание .circtmpl файла из .circ (все пути к файлам заменены на шаблоны).
        """
        s = open(self.CIRC_NAME, 'r').read()
        open(self.CIRCTMPL_NAME, 'w+').write(s.replace(self.cd_path, '%CD_PATH%'))

    def circ(self):
        """
        Создание .circ файла из .circtmpl с заменой всех шаблонов.
        """
        s = open(self.CIRCTMPL_NAME, 'r').read()
        open(self.CIRC_NAME, 'w+').write(s.replace('%CD_PATH%', self.cd_path))


def main():
    cd_path = os.getcwd()
    modules = [
        Logisim(cd_path)
    ]

    for target in sys.argv[1:]:
        found = False
        for module in modules:
            if module.try_exec(target):
                found = True
                break

        if not found:
            Module.errors_count += 1
            print(f"Target '{target}' not found.")

    Module.report()


if __name__ == "__main__":
    main()
