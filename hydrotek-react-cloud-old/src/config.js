export const appName = 'HydroTek'
const prodSubfolder = 'plants/'
const devSubFolder = ''

//set to false for production
export const buildForDev = true
export const devDefaultRoute = 'devices'
export const devDummyLogin = true

export const serverLocation = '/' + (buildForDev ? devSubFolder : prodSubfolder)

export const serverAPIEndpoint = 'api.php'