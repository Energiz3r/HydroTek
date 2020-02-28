import { connect } from 'react-redux'
import { setRoute } from '../actions/UIActions'
import { serverAPILocation } from '../config'
import Toggle from '../utils/Toggle'
import ReactTooltip from "react-tooltip"

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
      <div className="device-content">
          <h4>Devices</h4>

          <div className="device-container">

            <div className="device-option-container device-parent-container">
              <input type="text" placeholder="Device Label" data-tip="A friendly name to identify the device, eg. 'Front Room'"></input>
            </div>
            <div className="device-option-container">
              <label data-tip="Controls whether the device uploads sensor readings at all">Enable Online Logging</label>
              <Toggle isChecked={true} />
            </div>
            <div className="device-option-container">
              <label data-tip="Frequency, in minutes, of how often to upload status to server">Upload frequency (mins)</label>
              <input type="text" placeholder="Upload Frequency"></input>
            </div>
            <div className="device-option-container">
              <label data-tip="Address to send device alerts to (if enabled)">Alert email address</label>
              <input type="text" placeholder="Email address"></input>
            </div>
            <div className="device-option-container">
              <label data-tip="Disables the second set of inputs / outputs and hides second set of charts and views online">Single device mode</label>
              <Toggle isChecked={true} />
            </div>

            <div className="device-option-container device-ind-1">
              <label>Plant 1</label>
            </div>
            <div className="device-option-container device-ind-2">
              <label data-tip="Name the plant being monitored">Plant name</label>
              <input type="text" placeholder="Plant name"></input>
            </div>

            {/* TEMP */}
            <div className="device-option-container device-ind-2">
              <label data-tip="Enable / Disable the temp / humidity sensor">Temp / humidity sensor enable</label>
              <Toggle isChecked={true} />
            </div>
              <div className="device-option-container device-ind-3">
                <label data-tip="Turn off the lamp if this sensor reads too hot">Lamp over-temp shutoff</label>
                <Toggle isChecked={true} />
              </div>
                <div className="device-option-container device-ind-4">
                  <label>Max lamp temp</label>
                  <input type="text" placeholder="Max Temp"></input>
                </div>
              <div className="device-option-container device-ind-3">
                <label data-tip="Send an email if this sensor reads too hot">Hi temp email alert</label>
                <Toggle isChecked={true} />
              </div>
              <div className="device-option-container device-ind-3">
                <label data-tip="Sound the buzzer on the device if this sensor reads too hot">Hi temp sound alarm</label>
                <Toggle isChecked={true} />
              </div>
                <div className="device-option-container device-ind-4">
                  <label>Hi temp</label>
                  <input type="text" placeholder="Hi Temp"></input>
                </div>
              <div className="device-option-container device-ind-3">
                <label data-tip="Send an email if this sensor reads too cold">Lo temp email alert</label>
                <Toggle isChecked={true} />
              </div>
              <div className="device-option-container device-ind-3">
                <label data-tip="Sound the buzzer on the device if this sensor reads too cold">Lo temp sound alarm</label>
                <Toggle isChecked={true} />
              </div>
                <div className="device-option-container device-ind-4">
                  <label>Lo temp</label>
                  <input type="text" placeholder="Lo Temp"></input>
                </div>



          </div>

          <ReactTooltip place="top" type="dark" effect="solid" />

          <button className='button-default' onClick={this.onDeviceAddClick}>Add Device</button>

      </div>
    )
  }
}

export default connect(state=>state)(Listing)