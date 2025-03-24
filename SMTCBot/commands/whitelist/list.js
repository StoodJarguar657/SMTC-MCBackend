const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const FileReaderManager = require("../../src/managers/FileReaderManager");
const Table = require("cli-table3");

const { convertTo2DArray } = require("../../src/Utils");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("list")
        .setDescription("Lists all whitelisted players")
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const response = await FileReaderManager.ReadFileFromServerJSON("whitelist.json")
        const players2D = convertTo2DArray(response.map(value => value.name), 5)

        const tbl = new Table({
            head: [],
            style: {
                head: [],
                border: []
            }
        })

        tbl.push(...players2D)
        
        let message = "**Currently whitelisted players:**\n\n```\n" + tbl.toString() + "\n```"
        await interaction.reply({ content: message, flags: MessageFlags.Ephemeral })
    }
}