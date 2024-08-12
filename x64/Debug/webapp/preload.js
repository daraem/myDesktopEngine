const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  receiveData: (callback) => ipcRenderer.on('data-from-main', callback)
});