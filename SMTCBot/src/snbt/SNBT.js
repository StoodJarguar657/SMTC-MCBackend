// TODO: Get a proper SNBT parser
const { exec } = require('child_process')

module.exports = {
    async Parse(data) {
        return new Promise((resolve, reject) => {
            exec(`py src/snbt/Script.py \"${data.replaceAll("\"", "\\\"")}\"`, (err, stdOut, stdErr) => {
                if(err) return reject(err)
                if(stdErr) return reject(stdErr)

                return resolve(JSON.parse(stdOut))
            })
        })
    }
}