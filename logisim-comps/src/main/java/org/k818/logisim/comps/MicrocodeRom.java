package org.k818.logisim.comps;

import com.cburch.logisim.data.Attribute;
import com.cburch.logisim.data.Attributes;
import com.cburch.logisim.data.Bounds;
import com.cburch.logisim.data.Value;
import com.cburch.logisim.instance.InstanceFactory;
import com.cburch.logisim.instance.InstancePainter;
import com.cburch.logisim.instance.InstanceState;
import com.cburch.logisim.instance.Port;
import com.cburch.logisim.util.StringUtil;

public class MicrocodeRom extends InstanceFactory {
    private final int PORT_MI2 = 0;
    private final int PORT_MIP = 1;
    private final int PORT_RELOAD = 2;
    private final int PORT_MI1 = 3;

    private final Attribute<String> pathAttr = Attributes.forString("File path");

    public MicrocodeRom() {
        super("K8-18 Microcode ROM", StringUtil.constantGetter("K8-18 Microcode ROM"));

        setAttributes(new Attribute[]{pathAttr},
                new Object[]{""});

        setOffsetBounds(Bounds.create(0, 0, 110, 70));

        var ports = new Port[]{
                // mi2
                new Port(110, 40, Port.OUTPUT, 24),
                // mip
                new Port(0, 30, Port.INPUT, 12),
                // reload
                new Port(30, 70, Port.INPUT, 1),
                // mi1
                new Port(110, 30, Port.OUTPUT, 32)
        };

        ports[PORT_MI1].setToolTip(StringUtil.constantGetter("Microinstruction output 1 [bits 0-31]"));
        ports[PORT_MIP].setToolTip(StringUtil.constantGetter("Microinstruction pointer (address)"));
        ports[PORT_RELOAD].setToolTip(StringUtil.constantGetter("Reloads content from file on disk on rising edge"));
        ports[PORT_MI2].setToolTip(StringUtil.constantGetter("Microinstruction output 2 [bits 32-55]"));

        setPorts(ports);
    }

    @Override
    public void paintInstance(InstancePainter painter) {
        painter.drawBounds();
        painter.drawLabel();
        painter.drawPorts();
    }

    @Override
    public void propagate(InstanceState state) {
        Value mip = state.getPort(PORT_MIP);
        Value reload = state.getPort(PORT_RELOAD);

        String path = state.getAttributeValue(pathAttr);
        var microcode = Microcode.get(state, path);
        if (!path.isEmpty())
            microcode.reloadIfNeeded(reload.toIntValue() == 1, path);

        if (mip.isFullyDefined()) {
            var mi = microcode.getByAddr(mip.toIntValue());
            state.setPort(PORT_MI1, mi.part1(), 1);
            state.setPort(PORT_MI2, mi.part2(), 1);
        }
    }
}
