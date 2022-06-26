package org.k818.logisim.comps;

import com.cburch.logisim.data.BitWidth;
import com.cburch.logisim.data.Value;
import com.cburch.logisim.instance.InstanceState;

class Microcode extends RomContents {
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

    public Microcode() {
        super();
    }

    public static Microcode get(InstanceState state, String sourcePath) {
        return (Microcode) getInstance(state, sourcePath, Microcode::new);
    }

    public MInstr getMInstrByAddr(int addr) {
        int start = addr * MINSTR_SIZE;
        Value firstByte = getValueByAddr(start);
        if (firstByte.isErrorValue())
            return MInstr.createError();

        // Чтение первой части.
        int part1 = 0;
        for (int i = 0, shift = 0; i < 4; i++, shift += 8) {
            Value v = getValueByAddr(i + start);
            if (v.isErrorValue())
                return MInstr.createError();

            part1 |= v.toIntValue() << shift;
        }

        // Чтение второй части.
        int part2 = 0;
        for (int i = 0, shift = 0; i < 3; i++, shift += 8) {
            Value v = getValueByAddr(i + 4 + start);
            if (v.isErrorValue())
                return MInstr.createError();

            part2 |= v.toIntValue() << shift;
        }

        return MInstr.createForInt(part1, part2);
    }
}