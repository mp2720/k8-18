package org.k818.logisim.comps;

import com.cburch.logisim.data.*;
import com.cburch.logisim.instance.*;
import com.cburch.logisim.util.StringGetter;
import com.cburch.logisim.util.StringUtil;

public class Rom extends InstanceFactory {
    private static final int PORT_ADDRESS = 0;
    private static final int PORT_RELOAD = 1;
    private static final int PORT_DATA = 2;

    private static final StringGetter nameGetter = StringUtil.constantGetter("ROM for K8-18");

    private final Attribute<String> pathAttr = Attributes.forString("File path");

    public Rom() {
        super(nameGetter.get(), nameGetter);

        setAttributes(new Attribute[]{pathAttr, StdAttr.WIDTH},
                new Object[]{"", BitWidth.create(16)});

        setOffsetBounds(Bounds.create(0, 0, 110, 70));

        var ports = new Port[]{
                // address
                new Port(0, 30, Port.INPUT, StdAttr.WIDTH),
                // reload
                new Port(30, 70, Port.INPUT, 1),
                // data
                new Port(110, 30, Port.OUTPUT, 8)
        };

        ports[PORT_ADDRESS].setToolTip(StringUtil.constantGetter("ROM address"));
        ports[PORT_RELOAD].setToolTip(StringUtil.constantGetter("Reloads content from file on disk on rising"
                + " edge"));
        ports[PORT_DATA].setToolTip(StringUtil.constantGetter("Data output"));

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
        Value address = state.getPort(PORT_ADDRESS);
        Value reload = state.getPort(PORT_RELOAD);

        String path = state.getAttributeValue(pathAttr);
        var rom = RomContents.get(state, path);
        if (!path.isEmpty())
            rom.reloadIfNeeded(reload.toIntValue() == 1, path);

        if (address.isFullyDefined()) {
            var data = rom.getValueByAddr(address.toIntValue());
            state.setPort(PORT_DATA, data, 1);
        }
    }
}
