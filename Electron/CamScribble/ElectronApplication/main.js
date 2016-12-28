const {app, BrowserWindow,ipcMain} = require('electron');
const CamScribble = require('cam_scribble').CamScribble;


function areYouSureYouWantToExit(window) {
  const {dialog} = require('electron');

  var value = dialog.showMessageBox(window,{
    type:'question',
    message:'Are you sure you want to exit?',
    buttons:['Yes','No'],
    cancelId: -1
  });

  return (value == 0);
}

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

  calibrationWindow.on('close',() => {
    // /console.log('opening application');
    onCloseCallback();
  });

  calibrationWindow.on('closed',() => {
    win = null;
  })
}


function startCamScribble() {
  var canvasWindow = null;
  var controlsWindow = null;

  function openControlWindow() {
    controlsWindow = new BrowserWindow({
      width: 275,
      height: 550,
      titleBarStyle: 'hidden',
      resizable: true
    });

    controlsWindow.loadURL(`file://${__dirname}/CamScribbleControls.html`);

    controlsWindow.setMenu(null);

    controlsWindow.on('close',function(e) {
      if(areYouSureYouWantToExit(controlsWindow)) {
        canvasWindow.preventClose = false;
        canvasWindow.close();
      } else {
        e.preventDefault();
      }
    });


    controlsWindow.webContents.openDevTools();

    controlsWindow.on('closed',() => {
        win = null;
    });
  }

  function openBigCanvas() {
    canvasWindow = new BrowserWindow({
      width: 600,
      height: 625,
      titleBarStyle: 'hidden',
      resizable: false,
      frame: false
    });

    canvasWindow.preventClose = true;

    canvasWindow.loadURL(`file://${__dirname}/CamScribbleCanvas.html`);

    //canvasWindow.setMenu(null);
    //canvasWindow.webContents.openDevTools();

    canvasWindow.on('close',function(e) {
      if(canvasWindow.preventClose)
        e.preventDefault();
    });
    canvasWindow.on('closed',() => {
        win = null;
    });
  }

  openControlWindow();
  openBigCanvas();

  // Whenever we recieve an object from
  // the controls window, we pass it to
  // canvas window.
  ipcMain.on('cs-controls', (event, arg) => {
    canvasWindow.webContents.send('cs-controls',arg);
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
          app.quit();
          break;
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

ipcMain.on('debugging-logs',(event,arg) => {
  console.log(arg);
});

ipcMain.on('application-control-message',(event,arg) => {
  switch(arg) {
    case 'exit':
      app.quit();
      break;
  }
});
