import React, {Component} from "react";
import { init } from "../Services/GameService";

class Canvas extends Component {

  async componentDidMount() {
    init(this.refs.canvas as HTMLCanvasElement);
  }

  render() {
    return (
        <canvas tabIndex={0} ref="canvas" width={1600} height={1050} style={{ 
          width: "100%",
          height: "100vh",
          position: "absolute",
          left: 0,
          top: 0}} />
    )
  }
}

export default Canvas;