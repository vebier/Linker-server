const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid');
const emailModule = require('./email');

async function GetVarifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        uniqueId = uuidv4();
        console.log("uniqueId is ", uniqueId)
        let text_str =  '您的验证码为'+ uniqueId +'请三分钟内完成注册'
        //发送邮件
        let mailOptions = {
    from: 'zhaozekai1101@qq.com', // 必须与授权账号一致
    to: call.request.email,
    subject: '您的注册验证码 - 来自 Linker',

    html: `
        <div style="font-family: 'Helvetica Neue', Arial, sans-serif; max-width: 600px; margin: auto; padding: 20px; border: 1px solid #eeeeee; border-radius: 10px; background-color: #f9f9f9;">
        <div style="text-align: center; margin-bottom: 20px;">
            <h2 style="color: #1da1f2;">🔗 Linker</h2>
            <p style="color: #555; font-size: 16px;">欢迎加入，我们很高兴见到你！</p>
        </div>

        <div style="background-color: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); text-align: center;">
            <h3 style="color: #333;">输入以下验证码完成注册</h3>
            <div style="font-size: 24px; font-weight: bold; letter-spacing: 8px; color: #1da1f2; margin: 10px 0;">
            ${uniqueId.slice(0, 8).toUpperCase()}
            </div>
            <p style="color: #777; font-size: 14px;">
            验证码将在 <strong>3分钟</strong> 后失效，请尽快使用。
            </p>
        </div>

        <div style="margin-top: 20px; padding: 15px; background-color: #fff8e1; border-radius: 8px; font-size: 14px; color: #d8a00d;">
            <strong>💡 小提示：</strong> 此验证码用于注册你的 Linker 账户。请勿转发给他人。
        </div>

        <div style="text-align: center; margin-top: 20px; color: #aaa; font-size: 12px;">
            <p>这不是你？请忽略此邮件。</p>
            <p>&copy; 2025 Linker. 一个专注于简洁与安全的即时通讯工具。</p>
        </div>
        </div>
    `
    };
        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Success
        }); 
    }catch(error){
        console.log("catch error is ", error)
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Exception
        }); 
    }
}
function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VarifyService.service, { GetVarifyCode: GetVarifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        console.log('grpc server started')        
    })
}
main()