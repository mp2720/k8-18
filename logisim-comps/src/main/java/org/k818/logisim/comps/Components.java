package org.k818.logisim.comps;

import com.cburch.logisim.tools.AddTool;
import com.cburch.logisim.tools.Library;

import java.util.List;

//  org.k818.logisim.comps.Components
public class Components extends Library {
    private final List<AddTool> tools;

    public Components() {
        tools = List.of(new AddTool(new MicrocodeRom()));
    }

    @Override
    public String getDisplayName() {
        return "K8-18 Microcode ROM";
    }

    @Override
    public List<AddTool> getTools() {
        return tools;
    }
}
