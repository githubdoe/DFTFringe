/* do not display the "select components" window during installation */
function Component()
{
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
}


/* create start menu shortcut on windows */
Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/DFTFringe.exe", "@StartMenuDir@/DFTFringe.lnk");
        component.addOperation("CreateShortcut", "@TargetDir@/res/Help/help.html", "@StartMenuDir@/DFTFringe help.lnk");
        component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/uninstall.lnk");
    }
}
