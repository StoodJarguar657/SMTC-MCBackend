const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");
const Table = require("cli-table3");

const { convertTo2DArray } = require("../../src/Utils");
const fs = require("fs/promises");
const { randomUUID } = require("crypto");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("list")
        .setDescription("Shows all players currently in the server")
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {        
        const response = await RCONManager.SendCommand(`/list`)
        if(response.status !== "success")
            return await interaction.reply({ content: `Error while running command! ${response.message}`, flags: MessageFlags.Ephemeral })

        const players = response.message.split(" ").slice(10).join(" ").split(", ")
        const players2D = convertTo2DArray(players, 5)
        
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
                  
        await interaction.reply({ content: "**Players in the server:**", files: [filePath], flags: MessageFlags.Ephemeral })

        await fs.rm(filePath)
    }
}