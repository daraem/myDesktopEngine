const { app, BrowserWindow} = require('electron');
const fs = require('fs');
const path = require('path');

let win;

const createWindow = () => {
    win = new BrowserWindow({
        width: 0,
        height: 0,
        autoHideMenuBar: true,
        transparent: true,
        frame: false,
        skipTaskbar: true,
        alwaysOnTop: true,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
          }
    })

    win.loadFile('index.html');
    win.setIgnoreMouseEvents(true, {forward:true});
    win.maximize();
}

let rawClickData = null;
app.whenReady().then(() => {
    setInterval(() => {
        fs.readFile('./data.txt', 'utf8', (err, data) => {
            if (err) {
            console.error(err);
            return;
            }
            if(data != rawClickData) {
                rawClickData = data;
                win.webContents.send('data-from-main', { message: data });
                if(data == "quit") {
                    app.quit();
                }
            }
        });
    }, 1);
    createWindow()
})