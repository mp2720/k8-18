package org.k818.logisim.comps;

import com.cburch.logisim.data.BitWidth;
import com.cburch.logisim.data.Value;
import com.cburch.logisim.instance.InstanceData;
import com.cburch.logisim.instance.InstanceState;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;

public class RomContents implements Cloneable, InstanceData {
    private static final BitWidth dataBitWith = BitWidth.create(8);
    private byte[] bytes;
    private boolean loadingError = false;
    /**
     * Предыдущее значение входа reload.
     */
    private boolean prevReloadValue = false;

    protected interface ContentsFactory {
        RomContents create();
    }

    protected static RomContents getInstance(InstanceState state, String sourcePath, ContentsFactory factory) {
        RomContents ret = (RomContents) state.getData();
        if (ret == null) {
            // Создание микрокода если его еще нет.
            ret = factory.create();
            ret.reloadIfNeeded(true, sourcePath);
            ret.prevReloadValue = false;
            state.setData(ret);
        }

        return ret;
    }

    /**
     * Получение микрокода из состояния ПЗУ (создание нового в случае необходимости).
     */
    public static RomContents get(InstanceState state, String sourcePath) {
        return getInstance(state, sourcePath, RomContents::new);
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

    public Value getValueByAddr(int addr) {
        if (bytes == null || addr >= bytes.length)
            return Value.createKnown(dataBitWith, 0);

        if (loadingError)
            return Value.createError(dataBitWith);

        return Value.createKnown(dataBitWith, bytes[addr]);
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
