import { connect } from "react-redux";
import { setRoute } from "../actions/UIActions";

class Support extends React.Component {
  constructor(props) {
    super(props);
    this.state = {};
  }
  render() {
    return (
      <div>
        <h1>Support</h1>
        <p>
          <a href="https://github.com/Energiz3r/HydroTek">
            https://github.com/Energiz3r/HydroTek
          </a>
        </p>
        <p>
          <a href="https://noobs.wtf">https://noobs.wtf</a>
        </p>
      </div>
    );
  }
}

export default connect((state) => state)(Support);
