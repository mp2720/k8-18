import os


class Logisim:
    MICROCODE_REL_PATH = 'mc.bin'
    JAR_REL_PATH = 'logisim-comps/target/k818-logisim-comps-1.0-SNAPSHOT.jar'

    def __init__(self, cd_path: str):
        self.cd_path = cd_path

    def template(self, circ_path: str, circtmpl_path: str):
        """
        Создание .circtmpl файла из .circ (все пути к файлам заменены на шаблоны).
        """
        s = open(circ_path, 'r').read()

        # Замена пути к микрокоду.
        s = s.replace(f'{self.cd_path}/{self.MICROCODE_REL_PATH}', '%MICROCODE_PATH%')
        # Замена пути к JAR компонентов.
        s = s.replace(f'{self.cd_path}/{self.JAR_REL_PATH}', '%JAR_PATH%')

        open(circtmpl_path, 'w+').write(s)

    def circ(self, circtmpl_path, circ_path: str):
        """
        Создание .circ файла из .circtmpl с заменой всех шаблонов.
        """
        s = open(circtmpl_path, 'r').read()

        # Замена пути к микрокоду.
        s = s.replace('%MICROCODE_PATH%', f'{self.cd_path}/{self.MICROCODE_REL_PATH}')
        # Замена пути к JAR компонентов.
        s = s.replace('%JAR_PATH%', f'{self.cd_path}/{self.JAR_REL_PATH}')

        open(circ_path, 'w+').write(s)


if __name__ == "__main__":
    # TODO: добавить функции, сделать обработку argv.
    logi = Logisim(os.getcwd())
    logi.circ('cpu.circtmpl', 'cpu.circ')
