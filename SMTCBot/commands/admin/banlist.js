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
        const bannedPlayersData = await FileReaderManager.ReadFileFromServerJSON(interaction.member.id, "banned-players.json")
        const bannedIpsData = await FileReaderManager.ReadFileFromServerJSON(interaction.member.id, "banned-ips.json")

        if(bannedIpsData.status !== "success")
            return await interaction.reply({ content: bannedIpsData.message, flags: MessageFlags.Ephemeral })

        if(bannedPlayersData.status !== "success")
            return await interaction.reply({ content: bannedPlayersData.message, flags: MessageFlags.Ephemeral })

        const tbl = new Table({
            head: ["Banned User/IP", "Banned by", "Expiration", "Reason for ban"],
            style: {
                head: [],
                border: []
            }
        })

        bannedIpsData.data.forEach(value => tbl.push([value.ip, value.source, value.expires, value.reason]))
        bannedPlayersData.data.forEach(value => tbl.push([value.name, value.source, value.expires, value.reason]))

        const filePath = `temp/${randomUUID()}.txt`
        await fs.writeFile(filePath, tbl.toString(), "utf8")
    
        await interaction.reply({ content: "**Ban list:**", files: [filePath], flags: MessageFlags.Ephemeral })
    
        await fs.rm(filePath)
    }
}