export const appName = "HydroTek"; //displays on the app
const prodSubfolder = "hydrotek/"; //the folder inside the hosted domain where this app will sit, eg. if the address is noobs.wtf/plants then this value will be set to 'plants/' - trailing slash necessary, unless it is the root folder, then it should simply be ''
const devSubFolder = ""; //the folder inside the hosted domain where this app will sit, eg. if the address is noobs.wtf/plants then this value will be set to 'plants/' - trailing slash necessary, unless it is the root folder, then it should simply be ''

export const facebookAppID = 5666685910071027;

//set to false for production
export const buildForDev = true;
export const devDefaultRoute = "dashboard";
export const devDummyLogin = true;

export const serverLocation =
  "/" + (buildForDev ? devSubFolder : prodSubfolder);

export const serverAPIEndpointFacebook = "api-login-fb.php";
export const serverAPIEndpointDeviceConfig = "api-device-config.php";
