const {app, BrowserWindow,ipcMain} = require('electron');
const CamScribble = require('cam_scribble').CamScribble;



function openCalibrationWindow(onCloseCallback) {
  var calibrationWindow = new BrowserWindow({
      width:800,
      height:800,
      titleBarStyle: 'hidden',
      resizable: true
  });

  calibrationWindow.loadURL(`file://${__dirname}/calibrateCamera.html`);

  calibrationWindow.setMenu(null);

  //calibrationWindow.webContents.openDevTools();

  calibrationWindow.on('closed',() => {
      win = null;
      onCloseCallback();
  });
}


function startCamScribble() {
  var controlsWindow = new BrowserWindow({
    width: 250,
    height: 550,
    titleBarStyle: 'hidden',
    resizable: false
  });

  controlsWindow.loadURL(`file://${__dirname}/CamScribbleControls.html`);

  controlsWindow.setMenu(null);

  controlsWindow.on('closed',() => {
      win = null;
  });
}


function checkForConfigurationAndStart() {
  var fs = require('fs');
  // If a configuration is found, ask the user.
  // else open the calibration window.
  fs.stat('calibration.config',function(err,stat) {
    if(err == null) {
      const {dialog} = require('electron');
      var value = dialog.showMessageBox({
        type:'question',
        buttons:['Continue','Recalibrate'],
        message: 'Continue with existing calibration or recalibrate?',
        cancelId: -1
      });

      switch(value) {
        case -1:
        case 0:
          startCamScribble();
          break;
        case 1:
          openCalibrationWindow(function() {
            startCamScribble();
          });
      }
    } else {
      openCalibrationWindow(function() {
        startCamScribble();
      });
    }
  });
}


function main() {
  checkForConfigurationAndStart();
}


app.on('ready',main);


app.on('window-all-closed',() => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
});


app.on('activate', () => {

    if(win==null) {
        main();
    }
});




ipcMain.on('a-message', (event, arg) => {
  console.log(arg)  // prints "ping"
  //event.returnValue = 'pong'
})
