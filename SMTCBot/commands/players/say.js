const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("say")
        .setDescription("Says something to the people in the server. This command is annoynomus of the /say command.")
        .addStringOption(option => option.setName("message").setDescription("The message to send").setRequired(true))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const message = interaction.options.getString("message") 
        const json = [
            "",
            {"text": "["},
            {"text": "Discord", "color": "blue"},
            {"text": "] "},
            {
                "text": (interaction.member.nickname || interaction.member.displayName),
                "color": "gray",
                "underlined": true,
                "clickEvent": {
                    "action": "open_url",
                    "value": `https://discord.com/users/${interaction.member.id}`
                },
                "hoverEvent": {
                    "action": "show_text",
                    "contents":[
                        "",
                        {"text": "Press this to go to "},
                        {"text": (interaction.member.nickname || interaction.member.displayName), "bold":true, "color":"blue"},
                        {"text":"'s discord profile!"}
                    ]
                }
            },
            {"text":`: ${message.replaceAll("\"", "\\\"")}`}
        ]

        const response = await RCONManager.SendCommand(interaction.member.id, `/tellraw @a ${JSON.stringify(json)}`)

        if(response.status === "success")
            return await interaction.reply({ content: "Sent message!", flags: MessageFlags.Ephemeral })

        await interaction.reply({ content: `Failed to send message! ${response.message}`, flags: MessageFlags.Ephemeral })
    }
}