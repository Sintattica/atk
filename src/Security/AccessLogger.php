<?php

namespace Sintattica\Atk\Security;

use Exception;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Core\Tools;

class AccessLogger extends SecurityListener
{
    protected ?Db $db;
    protected string $logTable;
    protected string $appName;

    public function __construct()
    {
        $this->db = Db::getInstance();
        $this->logTable = Config::getGlobal('auth_accesslog_table');
        $this->appName = Config::getGlobal('app_name');
    }

    protected function logEvent(string $username, string $eventType,
                                string $status = null, string $details = null): void
    {
        $data = [
            'timestamp' => date('Y-m-d H:i:s'),
            'app_name' => $this->appName,
            'username' => $username,
            'ip_address' => $_SERVER['REMOTE_ADDR'] ?? '0.0.0.0',
            'event_type' => $eventType,
            'user_agent' => $_SERVER['HTTP_USER_AGENT'] ?? null,
            'status' => $status,
            'details' => $details ? json_encode($details) : null
        ];

        $fields = implode(',', array_keys($data));
        $placeholders = implode(',', array_fill(0, count($data), '?'));

        $sql = "INSERT INTO `{$this->logTable}` ($fields) VALUES ($placeholders)";

        try {
            $stmt = $this->db->prepare($sql);
            $stmt->execute(array_values($data));
            $this->db->commit();
        } catch (Exception $e) {
            Tools::atkdebug("Error logging access event: " . $e->getMessage());
        }
    }

    public function postLogin(string $username, array $extra): void
    {
        $this->logEvent($username, 'LOGIN');
    }

    public function errorLogin(string $username, array $extra): void
    {
        $status = isset($extra['auth_response']) ?
            $this->getAuthResponseLabel($extra['auth_response']) : 'UNKNOWN';

        $details = null;
        if (isset($extra['fatal_error'])) {
            $details = ['error_message' => $extra['fatal_error']];
        }

        $this->logEvent($username, 'LOGIN_FAILED', $status, $details);
    }

    public function postLogout($username, array $extra): void
    {
        $this->logEvent($username, 'LOGOUT');
    }

    protected function getAuthResponseLabel(string $authResponse): string
    {
        $labels = [
            SecurityManager::AUTH_SUCCESS => 'SUCCESS',
            SecurityManager::AUTH_LOCKED => 'ACCOUNT_LOCKED',
            SecurityManager::AUTH_MISMATCH => 'INVALID_CREDENTIALS',
            SecurityManager::AUTH_MISSINGUSERNAME => 'MISSING_USERNAME',
            SecurityManager::AUTH_ERROR => 'ERROR',
            SecurityManager::AUTH_UNVERIFIED => 'UNVERIFIED'
        ];

        return $labels[$authResponse] ?? 'UNKNOWN';
    }

}