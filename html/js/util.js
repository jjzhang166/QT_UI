
var TableFieldProtobuffCfg = {
    'player': {},
    'task': {'datas': 'TaskProtocol'},
    'skill': {'datas': 'SkillProtocol'},
    'item': {'datas': 'PBItemGroup'},
    'achieveevent': {'datas': 'AchieveEventProtocol'},
    'activity':{'datas': 'ActivityProtocol'},
    'rolepvp':{'datas': 'PvpProtocol'},
    'stall':{'datas': 'StallProtocol'},
    'trade':{'datas': 'TradeProtocol'},
    'common':{'datas': 'CommonProtocol'},
    'buff':{'datas': 'BuffProtocol'},
    'digmine':{'datas': 'DigMineProtocol'},
    'ride':{'datas': 'RideProtocol'},
    'achieve':{'datas': 'AchieveProtocol'},
    'title':{'datas': 'TitleProtocol'},
    'adore':{'datas': 'AdoreProtocol'},
    'relation_flowerrd':{'datas': 'RelationFlowerProtocol'},
}

function getPBFieldProtoName(tableName, field){
    if (TableFieldProtobuffCfg.hasOwnProperty(tableName)){
        var l = TableFieldProtobuffCfg[tableName];
        if (l.hasOwnProperty(field)){
            return l[field];
        }
    }
    return '';
}
function data2table(tableName, dbRet){
    var html = '';
    for (var i = 0; i < dbRet.result.length; ++i){
        var row = dbRet.result[i];
        var ret = '<table class="table table-bordered"><tbody>';
        for (var j = 0; j < dbRet.fieldNames.length; ++j){
            var fieldName = dbRet.fieldNames[j];
            ret += '<tr><td>' + fieldName + '</td>';
            var pbName = getPBFieldProtoName(tableName, fieldName);
            if (pbName == ''){
                ret += '<td>' + QT.charestDecode(row[j]) + '</td></tr>';
            }
            else{
                var ret2 = QT.strFilter(row[j]);
                var data = QT.pbcToJs("dbbuff.pb", pbName, ret2);
                console.log('pbcToJs', data);
                var childTable = pb2table(data);
                ret += '<td>' + childTable + '</td></tr>';
            }
        }
        ret += '</tr></tbody></table>'
        html += ret;
    }
    return html;
}

function pb2table(pbdata){
    var html = '';JSON.stringify(pbdata);
    
    var ret = '<table class="table table-bordered"><tbody>';
    for (var fieldName in pbdata){
        var fieldData = pbdata[fieldName];
        ret += '<tr><td>' + fieldName + '</td>';
        
        if (typeof fieldData === 'object'){
            console.log('pb2table', fieldData);
            ret += '<td>' + pb2table(fieldData) + '</td></tr>';
        }
        else{
            ret += '<td>' + QT.charestDecode(fieldData) + '</td></tr>';
        }
        
    }
    ret += '</tr></tbody></table>'
    html += ret;
    return html;
}