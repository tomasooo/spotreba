import axios from 'axios'

export const getSpotreba = async () => {
    const res = await axios.get('http://localhost:8080/api/spotreba')
    return res.data
}

export const getSpotrebaByRange = async (od, do_) => {
    // Ověření, že vstupy jsou validní datové objekty
    if (!od || !do_ || isNaN(new Date(od)) || isNaN(new Date(do_))) {
        console.error("Neplatné hodnoty od/do:", { od, do_ })
        return []
    }

    // Funkce pro formátování do ISO bez sekund a 'Z'
    const format = (date) => {
        const d = new Date(date)
        return d.toISOString().slice(0, 19)
    }

    try {
        const res = await axios.get('http://localhost:8080/api/spotreba/range', {
            params: {
                od: format(od),
                do: format(do_)
            }
        })
        return res.data
    } catch (error) {
        console.error("Chyba při načítání dat z API /spotreba/range:", error)
        return []
    }
}