package org.k818.logisim.comps;

import com.cburch.logisim.tools.AddTool;
import com.cburch.logisim.tools.Library;

import java.util.List;

public class Components extends Library {
    private final List<AddTool> tools;

    public Components() {
        tools = List.of(new AddTool(new MicrocodeRom()), new AddTool(new Rom()));
    }

    @Override
    public String getDisplayName() {
        return "K8-18 Components";
    }

    @Override
    public List<AddTool> getTools() {
        return tools;
    }
}