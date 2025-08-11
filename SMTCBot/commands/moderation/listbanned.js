import { AttachmentBuilder, MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import serverManager from "../../src/serverManager.js"
import Table from "cli-table3";

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("list-banned")
        .setDescription("Lists all banned users"),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const bannedPlayersData = await serverManager.readFile(serverInfo, "banned-players.json")
        const bannedIpsData = await serverManager.readFile(serverInfo, "banned-ips.json")

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

        JSON.parse(bannedIpsData.message).forEach(value => tbl.push([value.ip, value.source, value.expires, value.reason]))
        JSON.parse(bannedPlayersData.message).forEach(value => tbl.push([value.name, value.source, value.expires, value.reason]))

        const tableOutput = tbl.toString();
        if (tableOutput.length <= (2000 - 8)) {
            return await interaction.reply({ content: `\`\`\`\n${tableOutput}\n\`\`\``, flags: MessageFlags.Ephemeral });
        }
    
        const buffer = Buffer.from(tableOutput, "utf-8");
        const attachment = new AttachmentBuilder(buffer, { name: "response.txt" });
        await interaction.reply({ files: [attachment], flags: MessageFlags.Ephemeral });
    }
}