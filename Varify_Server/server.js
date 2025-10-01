const grpc = require('@grpc/grpc-js');
const message_proto = require('./proto');
const const_module = require('./const');
const { v4: uuidv4 } = require('uuid');
const emailModule = require('./email');
const redis_module = require('./redis');
const { logWithSection } = require('./logger');



async function GetVarifyCode(call, callback) {
    const email = call.request.email;

    try {
        const redisKey = const_module.code_prefix + email;

        const queryRes = await redis_module.GetRedis(redisKey);
        
        if (queryRes === null) {
            logWithSection(
                '查询redis验证码',
                'DEBUG',
                'Redis 中无缓存验证码，生成新验证码',
                { email }
            );
        } else {
            logWithSection(
                '查询redis验证码',
                'DEBUG',
                'Redis 中已存在验证码',
                { email, cachedCode: queryRes }
            );
        }

        let uniqueId = queryRes;

        if (uniqueId === null) {
            // ✅ 生成后立即转为大写
            uniqueId = uuidv4().substring(0, 6).toUpperCase();
            logWithSection(
                '验证码生成',
                'DEBUG',
                '新验证码',
                { email, code: uniqueId }
            );

            const bres = await redis_module.SetRedisExpire(redisKey, uniqueId, 600);

            if (!bres) {
                logWithSection(
                'Redis 存储验证码失败',
                'ERROR',
                '邮箱Key',
                { email }
            );
                return callback(null, { 
                    email, 
                    error: const_module.Errors.RedisErr 
                });
            }
            logWithSection(
                '验证码已存入 Redis',
                'INFO',
                '验证码',
                { email,code: uniqueId }
            );
        } 

        const mailOptions = {
            from: 'zhaozekai1101@qq.com',
            to: email,
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
                        ${uniqueId}
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

        const sendRes = await emailModule.SendMail(mailOptions);

        if (!sendRes) {
            logWithSection(
                '邮件发送失败',
                'ERROR',
                '发送邮箱',
                { email }
            );

            return callback(null, { 
                email, 
                error: const_module.Errors.SendFail 
            });
        }
       
        logWithSection(
                '邮件发送成功',
                'INFO',
                '发送邮箱',
                { email }
            );

        callback(null, { 
            email, 
            error: const_module.Errors.Success 
        });

    } catch (error) {
        logWithSection(
                '处理验证码请求时发生异常',
                'ERROR',
                '错误信息',
                { email: call.request.email, 
                  error: error.message,
                  stack: error.stack  }
            );

        callback(null, { 
            email: call.request.email, 
            error: const_module.Errors.Exception 
        });
    }
}

function main() {
    const server = new grpc.Server();
    server.addService(message_proto.VarifyService.service, { GetVarifyCode });
    
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        logWithSection(
                'gRPC 服务器已启动',
                'INFO',
                '服务地址',
                { address: '0.0.0.0:50051' }
            );
    });
}

main();