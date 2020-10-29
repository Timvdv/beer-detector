import React, { useState } from 'react';
import { useSpeechRecognition } from 'react-speech-kit';

const keywords = ["beer", "alcohol", "drinks", "hangover"]
let calledBeer = false;

function Speech() {
  const [value, setValue] = useState('');

  const { listen, listening, stop } = useSpeechRecognition({
    onResult: (result: any) => {
      const words = result.split(" ");
      const lastWord = words[words.length - 1]

      setValue(lastWord);

      if(keywords.some(keyword => lastWord.toLowerCase().includes(keyword))) {
          calledBeer && fetch("http://10.0.1.26/on")
            .then(response => response.json())
            .then(data => console.log(data));

          calledBeer = true;
          setTimeout( () => calledBeer = false, 2000);
      }
    },
  });

  const toggle = () => {
    if(listening) {
      stop();
      return;
    }

    listen();
  }

  return (
    <div>
      <div style={{
        position: "absolute",
         width: "100%",
         left: 0,
         top: 0,
         padding: "2em",
         fontSize: "1em",
         color: "gray",
         textAlign: "center",
         boxSizing: "border-box"
      }}>
        { value }
      </div>

      <button onClick={toggle} style={{
        fontSize: "18px",
        padding: "1em"
      }}>
        {listening ? "Stop" : "Start"} the detector
        <span role="img" aria-label="Microphone"> 🍺 </span>
      </button>

      {listening && <div> <br /> Go ahead I'm listening</div>}
    </div>
  );
}

export default Speech