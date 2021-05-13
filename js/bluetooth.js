let app = new Vue({
  el: '#app',
  data: {
    characteristic: null,
    data_adx_x: [],
    data_adx_y: [],
    data_adx_z: [],
    data_ntc: [],
    data_bpm: [],
    data_spo2: [],
    data_fsr_1: []
  },
  methods: {
    request: function (event) {
      navigator.bluetooth.requestDevice({
        acceptAllDevices: true,
        optionalServices: [0xffe0]
      }).then(device => {
        console.log(device)
        return device.gatt.connect()
      }).then(server => {
        console.log(server)
        return server.getPrimaryService(0xffe0)
      }).then(service => {
        console.log(service)
        return service.getCharacteristic(0xffe1)
      }).then(characteristic => {
        console.log(characteristic)
        characteristic.startNotifications().then(() => {
          characteristic.addEventListener('characteristicvaluechanged',this.handleCharacteristicValueChanged)
        })

        this.characteristic = characteristic
      }).catch(error => {
        console.log(error.message)
      })
    }, handleCharacteristicValueChanged: function(event) {
      // Els valors que ens ha de retornar l'arduino han de ser:
      // NTC,FSR1,FSR2,FSR3,MAX30102_BPM,MAX30102_BPM_SPO2,ADX_X,ADX_Y,ADX_Z
      var value = new TextDecoder().decode(event.target.value) // Recollim els valors del buffer en ascci per transformar-los en string
      var receiveSeparator = '\n';
      var receiveBuffer = '';

      var fsr_1 = 0;
      var adx = 0;
      var adx_x; 
      var adx_y; 
      var adx_z;
      var pulsioxometre; 
      var bpm;
      var spo2;
      var ntc = 0;
      for (const c of value) {
        if (c === receiveSeparator) {
          fsr_1 = ((/FSR_1: \d{1,3}/.test(receiveBuffer)) ? receiveBuffer.match(new RegExp(/FSR_1: \d{1,3}/)) : 0);
          if (fsr_1[0]){
            // Parsing FSR
            fsr_1 = fsr_1[0];
            fsr_1 = fsr_1.split(/FSR_1: /)[1];
            this.saveDataToDatabase("fsr",fsr_1);
            console.log(fsr_1);
          }  
          pulsioxometre = ((/BPM: -?\d{1,3}\tSPO2. -?\d{1,3}/.test(receiveBuffer)) ? receiveBuffer.match(new RegExp(/BPM: -?\d{1,3}\tSPO2. -?\d{1,3}/)) : 0 );
          if (pulsioxometre[0]) {
            // Parsing accelerometre
            pulsioxometre = pulsioxometre[0];
            bpm = pulsioxometre.split(/BPM: |\tSPO2: /)[1];
            spo2 = pulsioxometre.split(/\tSPO2:/)[1];
            this.saveDataToDatabase("bpm",bpm);
            this.saveDataToDatabase("spo2",spo2);
            console.log(pulsioxometre);
          }
          
          adx = ((/X: -?\d{1,3}\tY: -?\d{1,3}\tZ: -?\d{1,3}/.test(receiveBuffer)) ? receiveBuffer.match(new RegExp(/X: -?\d{1,3}\tY. -?\d{1,3}\tZ: -?\d{1,3}/)) : 0 );
          if (adx[0]) {
            // Parsing accelerometre
            adx = adx[0];
            adx_x = adx.split(/X:|\tY:|\tZ:/)[1];
            adx_y = adx.split(/\tY: |\tZ:/)[1];
            adx_z = adx.split(/\tZ: /)[1];
            value = adx_x + ' ' + adx_y + ' ' + adx_z;
            this.saveDataToDatabase("adx",value);
            console.log(adx);
          }

          ntc = ((/NTC: \d{1,3}/.test(receiveBuffer)) ? receiveBuffer.match(new RegExp(/NTC: \d{1,3}/)) : 0 );
          if (ntc[0]){
            // Parsing NTC
            ntc = ntc[0];
            ntc = ntc.split(/NTC: /)[1];
            this.saveDataToDatabase("ntc",ntc);
            console.log(ntc);
          } 
          receiveBuffer = '';
        } else {
          receiveBuffer += c;
          //console.log(receiveBuffer);
        }
      }

      // Mostrar patades
      if (fsr_1 !=0 ) this.data_fsr_1.push(fsr_1);
      new Chartist.Line('.ct-chart-fsr_1', { 
        labels: ['Valor FSR'], 
        series: [this.data_fsr_1]},{
        width: 600,
        height: 400,
        showArea: true
      })

      // Mostrar temperatura
      if (ntc != 0) this.data_ntc.push(ntc);
      new Chartist.Line('.ct-chart-ntc', {
        labels: ['Valor NTC'],
        series: [this.data_ntc]}, {
        width: 600,
        height: 400,
        low: 30
      });

      // Mostrar accelerometre
      if (adx_x !=0 ) this.data_adx_x.push(adx_x);
      if (adx_y !=0 ) this.data_adx_y.push(adx_y);
      if (adx_z !=0 ) this.data_adx_z.push(adx_z);
      new Chartist.Line('.ct-chart-adx', {
        labels: ["Valors ADX"],
        series: [this.data_adx_x, this.data_adx_y, this.data_adx_z]}, {
        width: 600,
        height: 400,
        showArea: true,
        showLine: false,
        showPoint: false,
        fullWidth: true,
        axisX: {
          showLabel: true,
          showGrid: true
        }
      });

      // Mostrar BPM 
      if (bpm) this.data_bpm.push(bpm);
      new Chartist.Line('.ct-chart-bpm', {
        labels: ['BPM'],
        series: [this.data_bpm]}, {
        lineSmooth: Chartist.Interpolation.step(),
        width: 600,
        height: 400,
        low: 0
      });
      
      // Mostrar SPO2 
      if (spo2) this.data_spo2.push(spo2);
      new Chartist.Line('.ct-chart-spo2', {
        labels: ['SPo2'],
        series: [this.data_spo2]}, {
        width: 600,
        height: 400,
        lineSmooth: Chartist.Interpolation.simple(),
        showArea: true,
        low: 50
      });
    },
    // Enviem les dades per inserir a BBDD amb servei web PHP al nuvol
    saveDataToDatabase: function(type, data) {
      let body = JSON.stringify({ [type]: data })
      console.log(body)
      fetch("./service.php",  {
        method: "POST",
        body,
        headers: {"Content-type": "application/json;charset=UTF-8"},
        mode: 'no-cors'
      }).then(response => response.json()) .then(json => console.log(json)).catch(err => console.log(err));
    }
  }
})
