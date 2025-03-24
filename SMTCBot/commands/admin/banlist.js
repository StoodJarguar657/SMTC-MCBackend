const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const FileReaderManager = require("../../src/managers/FileReaderManager");
const Table = require("cli-table3");

const fs = require("fs/promises");
const { randomUUID } = require("crypto");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("banlist")
        .setDescription("Shows all players currently in the server")
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {        
        /** @type {{uuid: string, name: string, created: string, source: string, expires: string, reason: string}[]} */
        const bannedPlayersData = await FileReaderManager.ReadFileFromServerJSON("banned-players.json")

        /** @type {{ip: string, created: string, source: string, expires: string, reason: string}[]} */
        const bannedIpsData = await FileReaderManager.ReadFileFromServerJSON("banned-ips.json")

        const tbl = new Table({
            head: ["Banned User/IP", "Banned by", "Expiration", "Reason for ban"],
            style: {
                head: [],
                border: []
            }
        })

        bannedIpsData.forEach(value => tbl.push([value.ip, value.source, value.expires, value.reason]))
        bannedPlayersData.forEach(value => tbl.push([value.name, value.source, value.expires, value.reason]))

        const filePath = `temp/${randomUUID()}.txt`
        await fs.writeFile(filePath, tbl.toString(), "utf8")
    
        await interaction.reply({ content: "**Ban list:**", files: [filePath], flags: MessageFlags.Ephemeral })
    
        await fs.rm(filePath)
    }
}