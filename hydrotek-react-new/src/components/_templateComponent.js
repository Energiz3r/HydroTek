import { connect } from 'react-redux'
class A extends React.Component {
  constructor(props) {
    super(props)
    this.state = {

    }
  }
  render() {
    return (
      <div />
    )
  }
}

export default connect(state=>state)(A)