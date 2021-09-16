import { connect } from "react-redux";
import { setRoute } from "../actions/UIActions";
import ReactTooltip from "react-tooltip";
import { demoDevices } from "../utils/demo-device";
import { DashboardDeviceTile } from "./DashboardDeviceTile";

class DeviceDashboard extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      deviceList: demoDevices,
    };
  }
  componentDidMount = () => {
    this.setState({
      waitingForSave: true,
    });
  };
  onDeviceAddClick = () => {
    //create a device on the server and fetch the config
  };

  render() {
    const { deviceList } = this.state;

    return (
      <div className="dashb-content">
        <h2>Devices Dashboard</h2>

        <div className="dashb-tile-container">
          {deviceList.map((device, i) => (
            <DashboardDeviceTile device={device} key={i} />
          ))}
        </div>

        <ReactTooltip place="top" type="dark" effect="solid" />

        <button className="button-default" onClick={this.onDeviceAddClick}>
          +
        </button>
      </div>
    );
  }
}

export default connect((state) => state)(DeviceDashboard);
