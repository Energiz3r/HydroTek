export const setLocation = (location) => {
  return({
    type: 'SET_LOCATION',
    location
  })
}

export const setLastLocation = (location) => ({
  type: 'SET_LAST_LOCATION',
  location
})
