const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const FileReaderManager = require("../../src/managers/FileReaderManager")
const Table = require("cli-table3");

const fs = require("fs/promises");
const { randomUUID } = require("crypto");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("op-list")
        .setDescription("Shows you all defined operators")
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const data = await FileReaderManager.ReadFileFromServerJSON(interaction.member.id, "ops.json")
        if(data.status !== "success")
            return await interaction.reply({ content: data.message, flags: MessageFlags.Ephemeral })

        const operators = data.data.map((player) => [player.uuid, player.name, player.level, player.bypassesPlayerLimit ? "Yes" : "No"])

        const tbl = new Table({
            head: ["Player UUID", "Player Name", "Operator Level", "Bypasses player limit?"],
            style: {
                head: [],
                border: []
            }
        })
        
        tbl.push(...operators)

        const filePath = `temp/${randomUUID()}.txt`
        await fs.writeFile(filePath, tbl.toString(), "utf8")
            
        await interaction.reply({ content: "**Currently defined operators**", files: [filePath], flags: MessageFlags.Ephemeral })

        await fs.rm(filePath)
    }
}