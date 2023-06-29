/* do not display the "select components" window during installation */
function Component()
{
    installer.addWizardPageItem( component, "DesktopShortcutCheckBoxForm", QInstaller.StartMenuSelection );
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
}

/* create start menu shortcut on windows */
Component.prototype.createOperations = function()
{
    component.createOperations();
    try {
        if (systemInfo.productType === "windows") {
            /* Add start menu shortcut */
            component.addOperation("CreateShortcut", "@TargetDir@/DFTFringe.exe", "@StartMenuDir@/DFTFringe.lnk");
            component.addOperation("CreateShortcut", "@TargetDir@/res/Help/help.html", "@StartMenuDir@/DFTFringe help.lnk");
            component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/uninstall.lnk");

            /* Add desktop shortcut */
            var checkboxForm = component.userInterface( "DesktopShortcutCheckBoxForm" );
            if (checkboxForm && checkboxForm.desktopShortcutCheckBox.checked) {
                component.addOperation("CreateShortcut", "@TargetDir@/DFTFringe.exe", "@DesktopDir@/DFTFringe.lnk");
            }
        }
    } catch(e) {
        console.log(e);
    }
}
