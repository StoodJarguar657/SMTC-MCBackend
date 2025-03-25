const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const FileReaderManager = require("../../src/managers/FileReaderManager");
const Table = require("cli-table3");

const { convertTo2DArray } = require("../../src/Utils");
const fs = require("fs/promises");
const { randomUUID } = require("crypto");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("list")
        .setDescription("Lists all whitelisted players")
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const response = await FileReaderManager.ReadFileFromServerJSON(interaction.member.id, "whitelist.json")
        if(response.status !== "success")
            return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })

        const players2D = convertTo2DArray(response.data.map(value => value.name), 5)

        const tbl = new Table({
            head: [],
            style: {
                head: [],
                border: []
            }
        })

        tbl.push(...players2D)
        
        const filePath = `temp/${randomUUID()}.txt`
        await fs.writeFile(filePath, tbl.toString(), "utf8")
        
        let message = "**Currently whitelisted players:**"
        await interaction.reply({ content: message, files: [filePath], flags: MessageFlags.Ephemeral })

        await fs.rm(filePath)
    }
}