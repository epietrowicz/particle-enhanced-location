import axios from 'axios'
import dotenv from 'dotenv'

dotenv.config()

const accessToken = process.env.PARTICLE_ACCESS_TOKEN
const productId = process.env.PARTICLE_PRODUCT_ID
const deviceId = process.env.PARTICLE_DEVICE_ID

async function getDeviceLocation () {
  const url = `https://api.particle.io/v1/products/${productId}/locations/${deviceId}`

  try {
    const response = await axios.get(url, {
      headers: {
        Authorization: `Bearer ${accessToken}`
      }
    })

    console.log('Location data:', response.data)
  } catch (error) {
    if (error.response) {
      console.error('Error response:', error.response.data)
    } else {
      console.error('Error message:', error.message)
    }
  }
}

getDeviceLocation()
