import database from "./database.js";
import { promisify } from "util";

const getAsync = promisify(database.get).bind(database);
const BACKEND_SERVER_ADDRESS = "http://127.0.0.1:25576"

export default {
    /**
     * @param {import("discord.js").CommandInteraction} interaction 
     * @returns {Promise<import("serverInfo").ServerInfo?>}
     */
    async getServerInfo(interaction) {
        return await getAsync("SELECT * FROM servers WHERE guildId = ? AND channelId = ?", [interaction.guildId, interaction.channelId]);
    },

    /**
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {string} command 
     */
    async sendRcon(serverInfo, command) {
        const response = await fetch(`${BACKEND_SERVER_ADDRESS}/rconSend`, {
            method: "POST",
            headers: {
                "Authorization": serverInfo.rconPassword
            },
            body: JSON.stringify({
                serverName: serverInfo.identifier,
                message: command
            })
        })

        return await response.json()
    },

    /**
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {string} filePath
     */
    async readFile(serverInfo, filePath) {
        const response = await fetch(`${BACKEND_SERVER_ADDRESS}/readFile`, {
            method: "POST",
            headers: {
                "Authorization": serverInfo.rconPassword
            },
            body: JSON.stringify({
                serverName: serverInfo.identifier,
                filePath: filePath
            })
        })

        return await response.json()
    }
};
