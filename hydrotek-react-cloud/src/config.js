export const appName = 'HydroTek' //displays on the app
const prodSubfolder = 'plants/' //the folder inside the hosted domain where this app will sit, eg. if the address is noobs.wtf/plants then this value will be set to 'plants/' - trailing slash necessary, unless it is the root folder, then it should simply be ''
const devSubFolder = '' //the folder inside the hosted domain where this app will sit, eg. if the address is noobs.wtf/plants then this value will be set to 'plants/' - trailing slash necessary, unless it is the root folder, then it should simply be ''

export const facebookAppID = 5666685910071027
const facebookClientToken = '960a1563374825576ed04e91e3d98b72'

//set to false for production
export const buildForDev = false
export const devDefaultRoute = 'devices'
export const devDummyLogin = true

export const serverLocation = '/' + (buildForDev ? devSubFolder : prodSubfolder)

export const serverAPIEndpointFacebook = 'api-login-fb.php'
export const serverAPIEndpointDeviceConfig = 'api-device-config.php'