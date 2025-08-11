import { AttachmentBuilder, MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import serverManager from "../../src/serverManager.js"
import Table from "cli-table3";

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("list-ops")
        .setDescription("Lists all opped users"),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const data = await serverManager.readFile(serverInfo, "ops.json")
        if(data.status !== "success")
            return await interaction.reply({ content: data.message, flags: MessageFlags.Ephemeral })

        const operators = JSON.parse(data.message).map((player) => [player.uuid, player.name, player.level, player.bypassesPlayerLimit ? "Yes" : "No"])

        const tbl = new Table({
            head: ["Player UUID", "Player Name", "Operator Level", "Bypasses player limit?"],
            style: {
                head: [],
                border: []
            }
        })
        
        tbl.push(...operators)

        const tableOutput = tbl.toString();
        if (tableOutput.length <= (2000 - 8)) {
            return await interaction.reply({ content: `\`\`\`\n${tableOutput}\n\`\`\``, flags: MessageFlags.Ephemeral });
        }
    
        const buffer = Buffer.from(tableOutput, "utf-8");
        const attachment = new AttachmentBuilder(buffer, { name: "response.txt" });
        await interaction.reply({ files: [attachment], flags: MessageFlags.Ephemeral });
    }
}