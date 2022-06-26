package org.k818.logisim.comps;

import com.cburch.logisim.data.BitWidth;
import com.cburch.logisim.data.Value;
import com.cburch.logisim.instance.InstanceData;
import com.cburch.logisim.instance.InstanceState;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;

class Microcode implements Cloneable, InstanceData {
    private static final int SIZE = 4096;
    private static final int MINSTR_SIZE = 7;

    /**
     * @param part1 Первые 32 бита из 56.
     * @param part2 Последние 24 бита из 56.
     */
    record MInstr(Value part1, Value part2) {
        private static final BitWidth part1BitWidth = BitWidth.create(32);
        private static final BitWidth part2BitWidth = BitWidth.create(24);

        public static MInstr createError() {
            return new MInstr(Value.createError(part1BitWidth), Value.createError(part2BitWidth));
        }

        public static MInstr createForInt(int part1, int part2) {
            return new MInstr(Value.createKnown(part1BitWidth, part1), Value.createKnown(part2BitWidth, part2));
        }
    }

    private byte[] bytes;
    private boolean loadingError = false;
    /**
     * Предыдущее значение входа reload.
     */
    private boolean prevReloadValue = false;

    public Microcode() {
        bytes = new byte[SIZE * MINSTR_SIZE];
    }

    /**
     * Получение микрокода из состояния ПЗУ (создание нового в случае необходимости).
     */
    public static Microcode get(InstanceState state, String sourcePath) {
        Microcode ret = (Microcode) state.getData();
        if (ret == null) {
            // Создание микрокода если его еще нет.
            ret = new Microcode();
            ret.reloadIfNeeded(true, sourcePath);
            ret.prevReloadValue = false;
            state.setData(ret);
        }

        return ret;
    }

    /**
     * Перезагрузка содержимого ПЗУ из файла в случае необходимости.
     *
     * @param reloadPinValue текущее значение пина перезаргузки.
     */
    public void reloadIfNeeded(boolean reloadPinValue, String sourcePath) {
        try {
            // Перезагрузка возможно только при переходе значения входа reload от false к true.
            if (reloadPinValue && !prevReloadValue) {
                bytes = Files.readAllBytes(new File(sourcePath).toPath());
                loadingError = false;
            }
        } catch (IOException e) {
            loadingError = true;
        } finally {
            prevReloadValue = reloadPinValue;
        }
    }

    public MInstr getByAddr(int addr) {
        if (addr < 0 || addr >= SIZE || loadingError)
            return MInstr.createError();

        int i = addr * MINSTR_SIZE;

        int part1 = (bytes[i] & 0xff) | (bytes[i + 1] & 0xff) << 8 | (bytes[i + 2] & 0xff) << 16 | (bytes[i + 3] & 0xff)
                << 24;
        int part2 = (bytes[i + 4] & 0xff) | (bytes[i + 5] & 0xff) << 8 | (bytes[i + 6] & 0xff) << 16;

        return MInstr.createForInt(part1, part2);
    }

    @Override
    public Object clone() {
        try {
            return super.clone();
        } catch (CloneNotSupportedException e) {
            return null;
        }
    }
}