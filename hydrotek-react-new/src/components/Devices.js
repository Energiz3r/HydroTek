import { connect } from 'react-redux'
import { setRoute } from '../actions/UIActions'
import { serverAPILocation } from '../config'
import Toggle from '../utils/Toggle'
import Tooltip from 'Tooltip'

class Listing extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      deviceList: [{
        UIdeviceVisible: false,
        deviceId: 1,
        singleDeviceMode: false,
        uploadFrequencyMins: 2,
        alertEmailAddress: "tim.eastwood@hotmail.com",
        plant1Name: 'Example',
        plant2Name: null
      }],
      waitingForAdd: false,
      loadingDevices: true
    }
  }
  onHomeClick = () => {
    this.props.dispatch(setRoute('/home'))
  }
  loadDeviceList = () => {
    fetch(serverAPILocation, {
      method: 'POST',
      cache: 'no-cache',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        "action": "getDeviceList"
      })
    })
      .then(result => result.json())
      .then(
        (result) => {
          if (result.deviceListAvailable === "1") {
            console.log(result)
            //handle response
          } else {
            console.log("Failed create account:")
            console.log(result)
          }
        },
        (error) => {
          console.log("Error:")
          console.log(error)
        }
      )
  }
  onDeviceAddClick = () => {
    //create a device on the server and fetch the config
  }
  render() {
    const { deviceList } = this.state
    return (
      <div>
          <h4>Devices</h4>
          <p>Hello. Route is: {this.props.location.pathname}</p>
          
          <div className="device-container">

            <input type="text" placeholder="Device Label"></input>
            <Tooltip Title="Controls whether the device uploads sensor readings">
              <label>Enable Logging</label>
            </Tooltip>
            <Toggle isChecked={true} />
            <Tooltip Title="Disables the second set of inputs / outputs and hides second set of charts and views online">
              <label>Single device mode</label>
            </Tooltip>
            <Toggle isChecked={true} />
            <Tooltip Title="Frequency, in minutes, of how often to upload status to server">
              <label>Upload frequency (mins)</label>
            </Tooltip>
            <input type="text" placeholder="Upload Frequency"></input>
            <Tooltip Title="Address to send device alerts to (if enabled)">
              <label>Alert email address</label>
            </Tooltip>
            <input type="text" placeholder="Email address"></input>

          </div>

          <button className='button-default' onClick={this.onDeviceAddClick}>Add Device</button>

      </div>
    )
  }
}

export default connect(state=>state)(Listing)