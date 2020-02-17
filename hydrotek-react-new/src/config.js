//set to false for production
export const buildForDev = true
export const useDummyLogin = true //affects dev only
export const stopAtLoginResponse = false //affects dev only
export const simulateCreateAccount = false //affects dev only
export const webpackDevServer = buildForDev
const prodServerUrlSubfolder = 'plants'

export const serverAPILocation = 'api.php'
export const serverLocation = buildForDev ? '' : '/' + prodServerUrlSubfolder
export const serverImagePath = serverLocation + '/images/'
