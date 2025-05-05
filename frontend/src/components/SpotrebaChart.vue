<template>
  <div style="display: flex; flex-wrap: wrap; justify-content: center; gap: 20px;">
    <!-- LEVÁ STRANA – Notifikace + Výchozí graf -->
    <div class="chart-column">
      <!-- Notifikace -->
      <div class="notification-box">
        <h3>🔔 Nastavit upozornění</h3>
        <label>
          Typ hodnoty:
          <select v-model="selectedField">
            <option value="vykon_w">Výkon (kW)</option>
            <option value="proud_a">Proud (A)</option>
            <option value="napeti_v">Napětí (V)</option>
            <option value="spotreba_kwh">Spotřeba (kWh)</option>
          </select>
        </label>
        <br />
        <label>
          Limit:
          <input type="number" v-model="limitValue" />
        </label>
        <br />
        <button @click="limitStartTime = new Date()">Začít sledovat od teď</button>
        <br /><br />

      </div>

      <!-- Výchozí graf -->
      <div class="graph-box">
        <Line v-if="chartData.labels.length" :data="chartData" :options="optionsAktualni" />
      </div>
    </div>

    <!-- PRAVÁ STRANA – Výběr rozsahu + graf -->
    <div class="chart-column">
      <!-- Výběr rozsahu zabalený do boxu se stejným stylem -->
      <div class="notification-box">
        <h3>📅 Výběr časového rozsahu</h3>
        <div style="display: flex; flex-direction: column; gap: 10px;">
          <label>Od: <input type="datetime-local" v-model="fromDate" /></label>
          <label>Do: <input type="datetime-local" v-model="toDate" /></label>
          <div style="display: flex; gap: 10px; flex-wrap: wrap; justify-content: center;">
            <button @click="loadCustomRange">Načíst</button>
            <button @click="loadLast24h">Posledních 24h</button>
            <button @click="loadLast7d">Poslední týden</button>
          </div>
        </div>
      </div>


      <div class="graph-box">
        <Line v-if="customChartData.labels.length" :data="customChartData" :options="optionsVyber" />
      </div>
    </div>
  </div>

  <!-- Souhrnné hodnoty za posledních 24h -->
  <div style="display: flex; flex-wrap: wrap; justify-content: center;align-items: flex-start ; gap: 1% ; margin-top: 1%">
    <div class="summary-box">
      <div class="summary-inner">
        <h3>📊 Souhrn za posledních 24 hodin</h3>
        <ul>
          <li><strong>🔋 Celková spotřeba:</strong> <span class="value red">{{ totalSpotreba24h }} kWh</span></li>
          <li><strong>⚡ Průměrný výkon:</strong> <span class="value blue">{{ avgVykon24h.toFixed(2) }} kW</span></li>
          <li><strong>🔌 Průměrný proud:</strong> <span class="value green">{{ avgProud24h.toFixed(2) }} A</span></li>
          <li><strong>🔧 Průměrné napětí:</strong> <span class="value yellow">{{ avgNapeti24h.toFixed(2) }} V</span></li>
        </ul>
      </div>
    </div>

    <div class="summary-box">
      <div class="summary-inner">
        <h3>📜 Historie upozornění</h3>
        <ul>
          <li v-for="(n, index) in notifikaceLog" :key="index">
            <p><strong>{{ n.cas }}</strong>: <span>{{ n.zprava }}</span></p>
          </li>
        </ul>
      </div>
    </div>
  </div>

  <!-- Tlačítko pro stažení CSV -->
  <div style="text-align: center; margin-top: 20px;">
    <button @click="exportToCSV">⬇️ Stáhnout aktuální data (CSV)</button>
  </div>
</template>

<script setup>
import { Line } from 'vue-chartjs'
import { ref, onMounted } from 'vue'
import {
  Chart as ChartJS,
  Title, Tooltip, Legend,
  LineElement, CategoryScale, LinearScale, PointElement
} from 'chart.js'
import { getSpotreba, getSpotrebaByRange } from '../api'

ChartJS.register(Title, Tooltip, Legend, LineElement, CategoryScale, LinearScale, PointElement)

const limitValue = ref(null)
const selectedField = ref('vykon_w')
const limitStartTime = ref(null)
const notifikaceLog = ref([]) // ✅ Uchováváme historii upozornění

const chartData = ref({
  labels: [],
  datasets: [
    { label: 'Výkon (kW)', data: [], borderColor: '#007bff', borderWidth: 2, tension: 0.3, pointRadius: 1.5 },
    { label: 'Proud (A)', data: [], borderColor: '#28a745', borderWidth: 2, tension: 0.3, pointRadius: 1.5 },
    { label: 'Napětí (V)', data: [], borderColor: '#ffc107', borderWidth: 2, tension: 0.3, pointRadius: 1.5 },
    { label: 'Spotřeba (kWh)', data: [], borderColor: '#dc3545', borderWidth: 2, tension: 0.3, pointRadius: 1.5 }
  ]
})

const customChartData = ref({
  labels: [],
  datasets: [
    { label: 'Výkon (kW)', data: [], borderColor: '#007bff', borderWidth: 2, tension: 0.3, pointRadius: 1.5 },
    { label: 'Proud (A)', data: [], borderColor: '#28a745', borderWidth: 2, tension: 0.3, pointRadius: 1.5 },
    { label: 'Napětí (V)', data: [], borderColor: '#ffc107', borderWidth: 2, tension: 0.3, pointRadius: 1.5 },
    { label: 'Spotřeba (kWh)', data: [], borderColor: '#dc3545', borderWidth: 2, tension: 0.3, pointRadius: 1.5 }
  ]
})

const totalSpotreba24h = ref(0)
const avgVykon24h = ref(0)
const avgProud24h = ref(0)
const avgNapeti24h = ref(0)

const fromDate = ref('')
const toDate = ref('')





const updateChart = async () => {
  const data = await getSpotreba()
  const sorted = data
      .filter(d => new Date(d.datum) <= new Date())
      .sort((a, b) => new Date(a.datum) - new Date(b.datum))
      .slice(-100)

  chartData.value.labels = sorted.map(d =>
      new Date(d.datum).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' })
  )
  chartData.value.datasets[0].data = sorted.map(d => d.vykon_w)
  chartData.value.datasets[1].data = sorted.map(d => d.proud_a)
  chartData.value.datasets[2].data = sorted.map(d => d.napeti_v)
  chartData.value.datasets[3].data = sorted.map(d => d.spotreba_kwh)

  checkLimitExceeded(sorted)
  checkAutomaticRecommendations(sorted)
}




const loadCustomRange = async () => {
  if (!fromDate.value || !toDate.value) return
  const res = await getSpotrebaByRange(new Date(fromDate.value), new Date(toDate.value))
  fillCustomChart(res)
  calculateSummary(res)
  checkLimitExceeded(res)
  checkAutomaticRecommendations(res)
}




const loadLast24h = async () => {
  const now = new Date()
  const from = new Date(now.getTime() - 24 * 60 * 60 * 1000)
  const res = await getSpotrebaByRange(from, now)
  fillCustomChart(res)
  calculateSummary(res)
  checkLimitExceeded(res)
}

const loadLast7d = async () => {
  const now = new Date()
  const from = new Date(now.getTime() - 7 * 24 * 60 * 60 * 1000)
  const res = await getSpotrebaByRange(from, now)
  fillCustomChart(res)
  calculateSummary(res)
  checkLimitExceeded(res)
}

function fillCustomChart(data) {
  const sorted = data.sort((a, b) => new Date(a.datum) - new Date(b.datum))
  customChartData.value.labels = sorted.map(d =>
      new Date(d.datum).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })
  )
  customChartData.value.datasets[0].data = sorted.map(d => d.vykon_w)
  customChartData.value.datasets[1].data = sorted.map(d => d.proud_a)
  customChartData.value.datasets[2].data = sorted.map(d => d.napeti_v)
  customChartData.value.datasets[3].data = sorted.map(d => d.spotreba_kwh)
}

function calculateSummary(data) {
  const filtered = data.filter(d =>
      d.spotreba_kwh != null && d.vykon_w != null && d.proud_a != null && d.napeti_v != null
  )
  const count = filtered.length || 1

  totalSpotreba24h.value = (filtered.reduce((sum, d) => sum + d.spotreba_kwh, 0).toFixed(3))
  avgVykon24h.value = filtered.reduce((sum, d) => sum + d.vykon_w, 0) / count
  avgProud24h.value = filtered.reduce((sum, d) => sum + d.proud_a, 0) / count
  avgNapeti24h.value = filtered.reduce((sum, d) => sum + d.napeti_v, 0) / count
}

function checkLimitExceeded(data) {
  if (!limitValue.value || !selectedField.value || !limitStartTime.value) return

  const filtered = data.filter(d => new Date(d.datum) >= new Date(limitStartTime.value))
  const breached = filtered.find(d => d[selectedField.value] > limitValue.value)

  if (breached) {
    const typ = {
      vykon_w: 'Výkon',
      proud_a: 'Proud',
      napeti_v: 'Napětí',
      spotreba_kwh: 'Spotřeba'
    }

    const doporuceni = {
      vykon_w: 'Zvaž řízení spotřebičů ve špičce, např. přes HDO.',
      proud_a: 'Zkontroluj počet spotřebičů v provozu současně.',
      napeti_v: 'Nestabilní napětí – doporučujeme sledovat vývoj.',
      spotreba_kwh: 'Vyšší denní spotřeba – analyzuj časové špičky.'
    }

    const zprava = `⚠️ ${typ[selectedField.value]} překročil limit ${limitValue.value} – ` +
        `Naměřeno: ${breached[selectedField.value]} v ${new Date(breached.datum).toLocaleTimeString()}. ` +
        `${doporuceni[selectedField.value]}`

    notifikaceLog.value.unshift({ cas: new Date().toLocaleString(), zprava })
    alert(zprava)
    limitValue.value = null
  }
}

function exportToCSV() {
  const now = new Date()
  const filename = `spotreba_export_${now.toISOString().slice(0, 19).replace(/:/g, '-')}.csv`
  const rows = [['Datum', 'Výkon (kW)', 'Proud (A)', 'Napětí (V)', 'Spotřeba (kWh)']]

  for (let i = 0; i < chartData.value.labels.length; i++) {
    rows.push([
      chartData.value.labels[i],
      chartData.value.datasets[0].data[i],
      chartData.value.datasets[1].data[i],
      chartData.value.datasets[2].data[i],
      chartData.value.datasets[3].data[i],
    ])
  }

  const csvContent = rows.map(r => r.join(',')).join('\n')
  const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' })
  const url = URL.createObjectURL(blob)
  const link = document.createElement('a')
  link.href = url
  link.setAttribute('download', filename)
  document.body.appendChild(link)
  link.click()
  document.body.removeChild(link)
}

onMounted(() => {
  updateChart()
  setInterval(updateChart, 60000)
  loadLast24h()
  setInterval(loadLast24h, 60000)

})



const optionsAktualni = {
  responsive: true,
  maintainAspectRatio: false,
  plugins: {
    legend: { position: 'top' },
    title: { display: true, text: '📈 Aktuální spotřeba (posledních 100 záznamů)' },
    tooltip: { mode: 'index', intersect: false }
  },
  scales: {
    y: { title: { display: true, text: 'Hodnota' } },
    x: { title: { display: true, text: 'Čas' } }
  }
}

const optionsVyber = {
  responsive: true,
  maintainAspectRatio: false,
  plugins: {
    legend: { position: 'top' },
    title: { display: true, text: '📅 Spotřeba podle výběru' },
    tooltip: { mode: 'index', intersect: false }
  },
  scales: {
    y: { title: { display: true, text: 'Hodnota' } },
    x: { title: { display: true, text: 'Čas' } }
  }
}


function vypocitejPrumernouSpotrebu(data, poslednichNminut) {
  const cutoff = new Date(Date.now() - poslednichNminut * 60 * 1000)
  const recent = data.filter(d => new Date(d.datum) >= cutoff)
  if (!recent.length) return 0
  return recent.reduce((sum, d) => sum + d.spotreba_kwh, 0) / recent.length
}




function checkAutomaticRecommendations(data) {
  if (!Array.isArray(data) || !data.length) return

  const posledni = data[data.length - 1]
  const prumerSpotreba3h = vypocitejPrumernouSpotrebu(data, 3 * 60) // 3 hodiny = 180 min

  const doporučení = []

  if (posledni.vykon_w > 4000) {
    doporučení.push("⚠️ Vysoký odběr energie – zvaž řízení spotřebičů pomocí chytré zásuvky nebo HDO, nebo zvaž využití energie z baterie.")
  }

  if (prumerSpotreba3h > 0.3) {
    doporučení.push("📈 Vyšší spotřeba – doporučujeme provést analýzu spotřeby po hodinách.")
  }

  if (posledni.napeti_v < 215) {
    doporučení.push("⚡ Nízké napětí – doporučujeme ověřit stabilitu elektrické sítě nebo kontaktovat distributora.")
  }

  if (posledni.proud_a > 16) {
    doporučení.push("🔌 Vysoký proud – zkontroluj, kolik spotřebičů běží zároveň.")
  }


  if (posledni.vykon_w < 500 && prumernyVykonZa3Hodiny < 1000) {
    doporuceni.push("🔋 Nízká spotřeba – vhodný čas pro nabíjení baterie.");
  }
    // Pokud je nové doporučení, přidej do logu
  for (const zprava of doporučení) {
    notifikaceLog.value.unshift({ cas: new Date().toLocaleString(), zprava })
  }
}
</script>

<style scoped>
.chart-container {
  flex: 1 1 500px;
  max-width: 600px;
  height: 400px;
  margin-top: 5%;
}


.chart-column {
  flex: 1 1 500px;
  max-width: 600px;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.graph-box {
  height: 400px;
  background: white;
  padding: 10px;
  border-radius: 12px;
  box-shadow: 0 4px 8px rgba(0,0,0,0.05);
}

.notification-box {
  background: #f9f9f9;
  padding: 16px;
  border-radius: 12px;
  box-shadow: 0 4px 8px rgba(0,0,0,0.05);
}

.summary-box {
  width: 100%;
  max-width: 600px;
  margin: 0;
}

.summary-inner {
  background: #f9f9f9;
  padding: 24px;
  border-radius: 12px;
  box-shadow: 0 4px 8px rgba(0,0,0,0.1);
  text-align: center;
}

.summary-inner ul {
  list-style: none;
  padding: 0;
  font-size: 16px;
}

.summary-inner li {
  margin-bottom: 10px;
}

.value {
  font-weight: bold;
}

.value.red {
  color: #dc3545;
}

.value.blue {
  color: #007bff;
}

.value.green {
  color: #28a745;
}

.value.yellow {
  color: #ffc107;
}
</style>
