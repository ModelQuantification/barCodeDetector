# This Python file uses the following encoding: utf-8
import json
import base64
from tencentcloud.common import credential
from tencentcloud.common.profile.client_profile import ClientProfile
from tencentcloud.common.profile.http_profile import HttpProfile
from tencentcloud.common.exception.tencent_cloud_sdk_exception import TencentCloudSDKException
from tencentcloud.ocr.v20181119 import ocr_client, models


def detectBarCode(filename):
    with open(filename, 'rb') as f:  # 以二进制读取图片
        data = f.read()
        encodestr = base64.b64encode(data)  # 得到 byte 编码的数据
        img_base64 = str(encodestr, 'utf-8')
        # print(ret)  # 重新编码数据

    try:
        cred = credential.Credential(
            "AKIDifTjkYFTYNxYu8tQ4f4E9pRp2INEKSnl", "4KtGQN5YmmXJZ1FMGtHPIgsoIQIvIPM6")
        httpProfile = HttpProfile()
        httpProfile.endpoint = "ocr.tencentcloudapi.com"

        clientProfile = ClientProfile()
        clientProfile.httpProfile = httpProfile
        client = ocr_client.OcrClient(cred, "ap-shanghai", clientProfile)

        req = models.QrcodeOCRRequest()
        params = {
            "ImageBase64": img_base64
        }
        req.from_json_string(json.dumps(params))
        resp = client.QrcodeOCR(req)
        resp_str = resp.to_json_string()
        # resp_str是API调用得到的结果，为字符串类型
        ret = json.loads(resp_str)
        data = ret["CodeResults"][0]["Url"]
        # print(resp_str)
        print(ret["CodeResults"][0]["Url"])
        return data

    except TencentCloudSDKException as err:
        print(err)
        return "0"


if __name__ == "__main__":
    detectBarCode("/home/hi/Downloads/barCode-shupian-s.jpg")
