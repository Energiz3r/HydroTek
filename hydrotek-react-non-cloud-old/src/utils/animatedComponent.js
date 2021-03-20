import React from 'react';
import { FadeTransform } from 'react-animation-components'
const transitionSpeed = 150 //150

export default (Component) => {
  return class extends React.Component {
    state = {
      shouldRender: this.props.isMounted,
      fadeOut: false,
      isHeader: this.props.isHeader
    }

    componentDidUpdate(prevProps) {
      if (prevProps.isMounted && !this.props.isMounted) {
        this.setState({...this.state, fadeOut: true})
        setTimeout(
          () => this.setState({ shouldRender: false }),
          transitionSpeed
        )
      } else if (!prevProps.isMounted && this.props.isMounted) {
        setTimeout(
          () => this.setState({ shouldRender: true, fadeOut: false }),
          transitionSpeed
        )
      }
    }

    render() {
      return this.state.shouldRender ?
        this.state.isHeader ?
          <FadeTransform in={!this.state.fadeOut}
            transformProps={{enterTransform: 'translateY(0rem)',
            exitTransform: this.state.fadeOut ? 'translateY(4.5rem)' : 'translateY(-4.5rem)'}}>
            <Component {...this.props} />
          </FadeTransform>
        :
          <FadeTransform in={!this.state.fadeOut}
            transformProps={{enterTransform: 'translateX(0rem)',
            exitTransform: this.state.fadeOut ? 'translateX(-4.5rem)' : 'translateX(4.5rem)'}}>
            <Component {...this.props} />
          </FadeTransform>
      : null
    }
  }
}